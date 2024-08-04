#include "io_syscall.h"
#include "resources.h"
#include "dslib.h"
#include "file_system.h"
#include "strlib.h"

extern resource_queue_t *disk_queue;

extern array_t* open_files;

static struct
{
    char* name;
    fs_entity_t *fs;
} args;


static void fcreate_free(){
    if(args.name){
        free(args.name, NameLength);
    }
}


static void fcreate_error()
{
    fcreate_free();
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fcreate_success()
{
    fcreate_free();
    disk_queue->handler->cpu_state.eax = (uint32_t)args.fs;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fcreate_proc(){
    cpu_state_t* state = &disk_queue->handler->cpu_state;

    args.name = alloc(NameLength);
    args.fs = 0;

    if (!args.name)
    {
        fcreate_error();
        return;
    }

    dir_entity_t* dir = (dir_entity_t*) state->eax;

    str_copy_n((char*)state->edx, args.name, NameLength);

    if(state->ebx){
        dir_create(dir, args.name, &args.fs, fcreate_success, fcreate_error);
    }else{
        file_create(dir, args.name, &args.fs, fcreate_success, fcreate_error);
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

    thread_t* thread = get_current_thread();

    schedule_process_waiting(state);

    resource_queue_inque(disk_queue, thread, fcreate_proc);
}
