#include "io_syscall.h"
#include "resources.h"
#include "dslib.h"
#include "file_system.h"
#include "strlib.h"

extern resource_queue_t *disk_queue;

extern open_file_t* get_open_file(uint32_t pntr);

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
    char* pntr = args.list;
    for (uint32_t i = 0; i < args.n_files; i++)
    {
        fs_entity_t* fs = (fs_entity_t*) pntr;
        str_copy_n(fs->name, args.to, NameLength);
        args.to += NameLength;
        pntr += SectorSize;
    }
    
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

    open_file_t* op = get_open_file(state->eax);

    if(!op || op->fs->type != DIR_TYPE){
        flist_error();
        return;
    }

    args.n_files = ((dir_entity_t*)op->fs)->n_childs;
    args.list = alloc(args.n_files * NameLength);

    if(!args.list){
        flist_error();
        return;
    }

    dir_list((dir_entity_t*)op->fs, (fs_entity_t*)args.list, flist_success, flist_error);
}

void flist_handler(cpu_state_t* state){

    schedule_process_waiting(state);

    resource_queue_inque(disk_queue, get_current_thread(), flist_proc);
}
