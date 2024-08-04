#include "io_syscall.h"
#include "resources.h"
#include "dslib.h"
#include "file_system.h"
#include "strlib.h"

extern resource_queue_t *disk_queue;


static struct
{
    char* list;
    uint32_t n_files;
    char* to;
} args;

static void flist_free(){
    if(args.list){
        free(args.list, args.n_files*SectorSize);
    }
}

static void flist_error()
{
    flist_free();
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void flist_copy_names(){
    uint32_t current_cr3;
    asm("mov %%cr3, %0":"=r"(current_cr3));
    uint32_t th_cr3 = ((pcb_t*)disk_queue->handler->parent)->cr3;
    asm volatile("mov %0, %%cr3" :: "r"(th_cr3));

    char* pntr = args.list;
    for (uint32_t i = 0; i < args.n_files; i++)
    {
        fs_entity_t* fs = (fs_entity_t*) pntr;
        str_copy_n(fs->name, args.to, NameLength);
        args.to += NameLength;
        pntr += SectorSize;
    }

    asm volatile("mov %0, %%cr3" :: "r"(current_cr3));
    
}

static void flist_success()
{
    flist_copy_names();
    flist_free();
    disk_queue->handler->cpu_state.eax = 1;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void flist_proc(){
    cpu_state_t* state = &disk_queue->handler->cpu_state;

    fs_entity_t* fs = (fs_entity_t*)state->eax;

    if(!file_is_open(fs) || fs->type != DIR_TYPE){
        flist_error();
        return;
    }

    args.n_files = ((dir_entity_t*)fs)->n_childs;
    args.list = alloc(args.n_files * NameLength);

    if(!args.list){
        flist_error();
        return;
    }

    dir_list((dir_entity_t*)fs, (fs_entity_t*)args.list, flist_success, flist_error);
}

void flist_handler(cpu_state_t* state){
    thread_t* thread = get_current_thread();

    schedule_process_waiting(state);

    resource_queue_inque(disk_queue, thread, flist_proc);
}
