#include "io_syscall.h"
#include "resources.h"
#include "dslib.h"
#include "file_system.h"
#include "strlib.h"

extern resource_queue_t *disk_queue;

extern array_t* open_files;

extern open_file_t* get_open_file(uint32_t pntr);

static struct
{
    char* name;
    fs_entity_t *fs;
    open_file_t *of;
} args;


static void fcreate_free(){
    if(args.name){
        free(args.name, NameLength);
    }
}


static void fcreate_error()
{
    fcreate_free();
    if (args.fs)
    {
        free((char *)args.fs, SectorSize);
    }
    if(args.of)
    {
        free((char*)args.of, sizeof(open_file_t));
    }
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fcreate_success()
{
    fcreate_free();
    open_file_t *of = args.of;
    of->fs = args.fs;
    of->handler = disk_queue->handler;
    void **pntr = array_add(open_files, of);
    disk_queue->handler->cpu_state.eax = (uint32_t)pntr;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fcreate_proc(){
    cpu_state_t* state = &disk_queue->handler->cpu_state;

    args.name = alloc(NameLength);
    args.fs = (fs_entity_t *)alloc(SectorSize);
    args.of = (open_file_t*)alloc(sizeof(open_file_t));

    if (!args.name || !args.fs || !args.of)
    {
        fcreate_error();
        return;
    }

    dir_entity_t* dir;

    if(state->eax){
        open_file_t* op = get_open_file(state->eax);

        if(!op || op->fs->type != DIR_TYPE){
            fcreate_error();
            return;
        }

        dir = (dir_entity_t*)op->fs;

    }else{
        dir = 0;
    }

    str_copy_n((char*)state->edx, args.name, NameLength);

    if(state->ebx){
        dir_create(dir, args.name, args.fs, fcreate_success, fcreate_error);
    }else{
        file_create(dir, args.name, args.fs, fcreate_success, fcreate_error);
    }
}

void fcreate_handler(cpu_state_t* state){
    if (open_files->size >= open_files->capacity)
    {
        state->eax = 0;
        return;
    }

    if (disk_queue->queue->size >= disk_queue->queue->capacity)
    {
        state->eax = 0;
        return;
    }

    schedule_process_waiting(state);

    resource_queue_inque(disk_queue, get_current_thread(), fcreate_proc);
}
