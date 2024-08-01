#include "io_syscall.h"
#include "resources.h"
#include "file_system.h"
#include "libc.h"

extern resource_queue_t* disk_queue;

static struct
{
    uint32_t sector_cnt;
    char* data;
} args;

static void fwrite_free(){
    if(args.data){
        free(args.data, args.sector_cnt*SectorSize);
    }
}


static void fwrite_error(){
    fwrite_free();
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fwrite_success(){
    fwrite_free();
    disk_queue->handler->cpu_state.eax = 1;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fwrite_copy_data(char* from, uint32_t count){
    uint32_t current_cr3;
    asm("mov %%cr3, %0":"=r"(current_cr3));
    uint32_t th_cr3 = ((pcb_t*)disk_queue->handler->parent)->cr3;
    asm volatile("mov %0, %%cr3" :: "r"(th_cr3));

    mem_copy(from, args.data, count);
    
    asm volatile("mov %0, %%cr3" :: "r"(current_cr3));

}

static void fwrite_proc(){
    cpu_state_t* state = &disk_queue->handler->cpu_state;
    uint32_t pntr = state->eax;
    char *from = (char*) state->edx;
    uint32_t count = state->ecx;
    args.data = 0;

    fs_entity_t* fs = (fs_entity_t*)(pntr);

    if(!file_is_open(fs) || fs->type != FILE_TYPE){
        fwrite_error();
        return;
    }
    
    args.sector_cnt = math_ciel((float)count / SectorSize);
    args.data = alloc(args.sector_cnt*SectorSize);
    if(!args.data){
        fwrite_error();
        return;
    }

    fwrite_copy_data(from, count);

    file_write((file_entity_t*)fs, args.data, args.sector_cnt, count, fwrite_success, fwrite_error);
}

void fwrite_handler(cpu_state_t* state)
{
    uint32_t count = state->ecx;

    if(!count){
        state->eax = 0;
        return;
    }

    if(disk_queue->queue->size >= disk_queue->queue->capacity){
        state->eax = 0;
        return;
    }

    schedule_process_waiting(state);
    resource_queue_inque(disk_queue, get_current_thread(), fwrite_proc);
}