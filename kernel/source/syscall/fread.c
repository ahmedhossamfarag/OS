#include "io_syscall.h"
#include "resources.h"
#include "file_system.h"

extern resource_queue_t* disk_queue;
extern open_file_t* get_open_file(uint32_t pntr);

static struct
{
    char *to;
    uint32_t seek;
    uint32_t count;
    uint32_t disk_seek;
    uint32_t disk_count;
    char* data;
} args;

void fread_error(){
    if(args.data){
        free((char*)args.data, args.disk_count * SectorSize);
    }
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

void fread_move_data(){
    uint32_t start = args.seek - args.disk_seek * SectorSize;
    uint32_t end = start + args.count;
    char* to = args.to;
    for(char* c = args.data + start; c < c + end; c++){
        *to = *c;
        to++;
    }
}

void fread_success(){
    fread_move_data();
    if(args.data){
        free((char*)args.data, args.disk_count * SectorSize);
    }
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}


void fread_proc(){
    cpu_state_t* state = &disk_queue->handler->cpu_state;
    uint32_t pntr = state->eax;
    args.to = (char*) state->edx;
    args.seek = state->ebx;
    args.count = state->ecx;
    args.data = 0;

    open_file_t* op = get_open_file(pntr);

    if(!op || op->fs->type != FILE_TYPE){
        fread_error();
        return;
    }

    args.data = alloc(args.disk_count * SectorSize);

    if(!args.data){
        fread_error();
        return;
    }

    args.disk_seek = args.seek / SectorSize;
    args.disk_count = math_cielm(args.seek + args.count, SectorSize) - args.disk_seek;

    file_read((file_entity_t*)op->fs, args.data, args.disk_seek, args.disk_count, fread_success, fread_error);
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
