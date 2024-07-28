#include "io_syscall.h"
#include "resources.h"
#include "dslib.h"
#include "file_system.h"

extern resource_queue_t *disk_queue;

extern open_file_t* get_open_file(uint32_t pntr);


static void fdelete_error()
{
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fdelete_success()
{
    fclose_handler(&disk_queue->handler->cpu_state);
    disk_queue->handler->cpu_state.eax = 1;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}


static void fdelete_proc(){
    cpu_state_t* state = &disk_queue->handler->cpu_state;

    open_file_t* op = get_open_file(state->eax);

    if(!op){
        fdelete_error();
        return;
    }

    if(op->fs->type == FILE_TYPE){
        file_delete((file_entity_t*)op->fs, fdelete_success, fdelete_error);
    }else if(op->fs->type == DIR_TYPE){
        dir_delete((dir_entity_t*)op->fs, fdelete_success, fdelete_error);
    }else{
        fdelete_error();
    }
}

void fdelete_handler(cpu_state_t* state){

    schedule_process_waiting(state);

    resource_queue_inque(disk_queue, get_current_thread(), fdelete_proc);
}
