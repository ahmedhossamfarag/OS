#include "th_syscall.h"
#include "resources.h"
#include "file_system.h"
#include "io_syscall.h"
#include "libc.h"
#include "pagging.h"

extern resource_queue_t* disk_queue;

static struct
{
    uint32_t cr3;
    char* data;   
    uint32_t n_sectors;
    char* to;
} args;

static void pcreate_error(){
    if(args.data){
        free((char*)args.data, args.n_sectors * SectorSize);
    }
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void pcreate_move_data(){
    uint32_t current_cr3;
    asm("mov %%cr3, %0":"=r"(current_cr3));
    uint32_t th_cr3 = args.cr3;
    asm volatile("mov %0, %%cr3" :: "r"(th_cr3));

    mem_copy(args.data, args.to, args.n_sectors*SectorSize);

    asm volatile("mov %0, %%cr3" :: "r"(current_cr3));
}

static void pcreate_add(){
    cpu_state_t* state = &disk_queue->handler->cpu_state;
    pcb_t* parent = (pcb_t*) disk_queue->handler->parent;
    add_new_process(parent->pid, state->eax, args.cr3, state->edx, args.n_sectors * SectorSize);
}

static void pcreate_success(){
    pcreate_move_data();
    if(args.data){
        free((char*)args.data, args.n_sectors * SectorSize);
    }
    pcreate_add();
    disk_queue->handler->cpu_state.eax = 1;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}


static void pcreate_proc(){
    cpu_state_t* state = &disk_queue->handler->cpu_state;

    args.data = 0;

    fs_entity_t* fs = (fs_entity_t*) state->ebx;

    if(!file_is_open(fs) || fs->type != FILE_TYPE){
        pcreate_error();
        return;
    }

    args.cr3 = (uint32_t) get_available_pagging_dir();

    if(!args.cr3){
        pcreate_error();
        return;
    }
    args.to = 0;
    args.n_sectors = ((file_entity_t*)fs)->n_blocks;
    args.data = alloc(args.n_sectors * SectorSize);

    if(!args.data){
        pcreate_error();
        return;
    }
    
    file_read((file_entity_t*)fs, args.data, 0, args.n_sectors, pcreate_success, pcreate_error);
}

void process_create_handler(cpu_state_t* state)
{
    if(disk_queue->queue->size >= disk_queue->queue->capacity){
        state->eax = 0;
        return;
    }

   schedule_process_waiting(state);
   resource_queue_inque(disk_queue, get_current_thread(), pcreate_proc);
}

