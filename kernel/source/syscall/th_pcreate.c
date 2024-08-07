#include "th_syscall.h"
#include "resources.h"
#include "file_system.h"
#include "pagging.h"

extern void linker_load_elf(char* file, uint32_t cr3, uint32_t* entry, uint32_t* membegin, SUCC_ERR);

#define PROCESS_STACK_SIZE 0x1000
#define MEMORY_PADDING 0x100
extern resource_queue_t* disk_queue;

static struct
{
    uint32_t cr3;
    char* data;   
    uint32_t n_sectors;
    uint32_t entry;
    uint32_t membegin;
} args;

static void pcreate_error(){
    if(args.data){
        free((char*)args.data, args.n_sectors * SectorSize);
    }
    if(args.cr3){
        free_pagging_dir((uint32_t*) args.cr3);
    }
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void pcreate_add(){
    cpu_state_t* state = &disk_queue->handler->cpu_state;
    pcb_t* parent = (pcb_t*) disk_queue->handler->parent;

    uint32_t eip = args.entry;
    uint32_t membegin = args.membegin;
    uint32_t ebp = membegin + PROCESS_STACK_SIZE;
    membegin += PROCESS_STACK_SIZE + MEMORY_PADDING;

    add_new_process(parent->pid, state->eax, args.cr3, eip, ebp, membegin);
}

static void pcreate_success(){
    if(args.data){
        free((char*)args.data, args.n_sectors * SectorSize);
    }
    pcreate_add();
    disk_queue->handler->cpu_state.eax = 1;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void pcreate_link(){
    linker_load_elf(args.data, args.cr3, &args.entry, &args.membegin, pcreate_success, pcreate_error);
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
    args.n_sectors = ((file_entity_t*)fs)->n_blocks;
    args.data = alloc(args.n_sectors * SectorSize);

    if(!args.data){
        pcreate_error();
        return;
    }
    
    file_read((file_entity_t*)fs, args.data, 0, args.n_sectors, pcreate_link, pcreate_error);
}

void process_create_handler(cpu_state_t* state)
{
    if(disk_queue->queue->size >= disk_queue->queue->capacity){
        state->eax = 0;
        return;
    }

    thread_t* thread = get_current_thread();

    schedule_process_waiting(state);
    resource_queue_inque(disk_queue, thread, pcreate_proc);
}

