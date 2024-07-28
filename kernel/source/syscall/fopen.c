#include "io_syscall.h"
#include "resources.h"
#include "dslib.h"
#include "file_system.h"
#include "strlib.h"

resource_queue_t *disk_queue;

array_t *open_files;

static struct
{
    uint32_t len;
    char *name_splited;
    char **splits;
    fs_entity_t *fs;
    open_file_t *of;
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

static void fopen_success()
{
    fopen_free();
    open_file_t *of = args.of;
    of->fs = args.fs;
    of->handler = disk_queue->handler;
    void **pntr = array_add(open_files, of);
    disk_queue->handler->cpu_state.eax = (uint32_t)pntr;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fopen_next()
{
    args.splits++;
    if (*args.splits)
    {
        if (args.fs->type == DIR_TYPE)
        {
            file_open((dir_entity_t *)args.fs, *args.splits, args.fs, fopen_next, fopen_error);
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
    args.fs = (fs_entity_t *)alloc(SectorSize);
    args.of = (open_file_t*)alloc(sizeof(open_file_t));

    if (!args.splits || !args.name_splited || !args.fs || !args.of)
    {
        fopen_error();
        return;
    }

    if(!name){
        file_open(0, 0, args.fs, fopen_success, fopen_error);
        return;
    }

    str_split(name, '/', args.splits, args.name_splited);

    if (*args.splits)
    {
        file_open(0, *args.splits, args.fs, fopen_next, fopen_error);
    }
    else
    {
        fopen_error();
    }
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

    schedule_process_waiting(state);

    resource_queue_inque(disk_queue, get_current_thread(), fopen_proc);
}