#include "io_syscall.h"
#include "resources.h"
#include "dslib.h"
#include "file_system.h"
#include "strlib.h"

resource_queue_t *disk_queue;

extern array_t *open_files;

static struct
{
    uint32_t len;
    char *name_splited;
    char **splits;
    fs_entity_t *parent;
    fs_entity_t *fs;
} args;

static void fopen_free()
{
    if (args.name_splited)
        free(args.name_splited, args.len + 2);
    if (args.splits)
        free((char *)args.splits, (args.len + 2) * sizeof(char *));
}

static void fopen_error()
{
    fopen_free();
    if (args.fs)
    {
        file_close(args.fs, 0, 0);
    }
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fopen_success()
{
    fopen_free();
    disk_queue->handler->cpu_state.eax = (uint32_t)args.fs;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fopen_next()
{
    if(args.parent){
        file_close(args.parent, 0, 0);
    }
    if (*args.splits)
    {
        if (args.fs->type == DIR_TYPE)
        {
            args.parent = args.fs;
            char* name = *args.splits;
            args.splits ++;
            file_open((dir_entity_t *)args.parent, name, &args.fs, fopen_next, fopen_error);
        }
        else
        {
            fopen_error();
        }
    }
    else
    {
        fopen_success();
    }
}

static void fopen_proc()
{
    char *name = (char*) disk_queue->handler->cpu_state.eax;

    args.len = str_len(name);
    args.name_splited = alloc(args.len + 2);
    args.splits = (char **)alloc((args.len + 2) * sizeof(char *));
    args.parent = 0;
    args.fs = 0;

    if (!args.splits || !args.name_splited)
    {
        fopen_error();
        return;
    }

    if(!name){
        file_open(0, 0, &args.fs, fopen_success, fopen_error);
        return;
    }

    str_split(name, '/', args.splits, args.name_splited);
    file_open(0, 0, &args.fs, fopen_next, fopen_error);
}

void fopen_handler(cpu_state_t *state)
{
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

    resource_queue_inque(disk_queue, thread, fopen_proc);
}