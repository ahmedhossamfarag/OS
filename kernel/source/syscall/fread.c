#include "io_syscall.h"
#include "resources.h"
#include "file_system.h"
#include "libc.h"

extern resource_queue_t* disk_queue;

static struct
{
    char *to;
    uint32_t seek;
    uint32_t count;
    uint32_t disk_seek;
    uint32_t disk_count;
    char* data;
} args;

static void fread_error(){
    if(args.data){
        free((char*)args.data, args.disk_count * SectorSize);
    }
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fread_move_data(){
    uint32_t current_cr3;
    asm("mov %%cr3, %0":"=r"(current_cr3));
    uint32_t th_cr3 = ((pcb_t*)disk_queue->handler->parent)->cr3;
    asm volatile("mov %0, %%cr3" :: "r"(th_cr3));

    uint32_t start = args.seek - args.disk_seek * SectorSize;
    
    mem_copy(args.data + start, args.to, args.count);
    
    asm volatile("mov %0, %%cr3" :: "r"(current_cr3));
}

static void fread_success(){
    fread_move_data();
    if(args.data){
        free((char*)args.data, args.disk_count * SectorSize);
    }
    disk_queue->handler->cpu_state.eax = 1;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}


static void fread_proc(){
    cpu_state_t* state = &disk_queue->handler->cpu_state;
    uint32_t pntr = state->eax;
    args.to = (char*) state->edx;
    args.seek = state->ebx;
    args.count = state->ecx;
    args.data = 0;

    fs_entity_t* fs = (fs_entity_t*)pntr;

    if(!file_is_open(fs) || fs->type != FILE_TYPE){
        fread_error();
        return;
    }


    args.disk_seek = args.seek / SectorSize;
    args.disk_count = math_cielm(args.seek + args.count, SectorSize) - args.disk_seek;

    args.data = alloc(args.disk_count * SectorSize);

    if(!args.data){
        fread_error();
        return;
    }
    file_read((file_entity_t*)fs, args.data, args.disk_seek, args.disk_count, fread_success, fread_error);
}

void fread_handler(cpu_state_t* state)
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
   resource_queue_inque(disk_queue, get_current_thread(), fread_proc);
}
