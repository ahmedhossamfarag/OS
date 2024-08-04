#include "io_syscall.h"
#include "resources.h"
#include "dslib.h"
#include "file_system.h"

extern resource_queue_t *disk_queue;



static void fdelete_error()
{
    disk_queue->handler->cpu_state.eax = 0;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}

static void fdelete_success()
{
    disk_queue->handler->cpu_state.eax = 1;
    thread_awake(disk_queue->handler);
    resource_queue_deque(disk_queue);
}


static void fdelete_proc(){
    cpu_state_t* state = &disk_queue->handler->cpu_state;

    fs_entity_t* fs = (fs_entity_t*)state->eax;

    if(!file_is_open(fs)){
        fdelete_error();
        return;
    }

    if(fs->type == FILE_TYPE){
        file_delete((file_entity_t*)fs, fdelete_success, fdelete_error);
    }else if(fs->type == DIR_TYPE){
        dir_delete((dir_entity_t*)fs, fdelete_success, fdelete_error);
    }else{
        fdelete_error();
    }
}

void fdelete_handler(cpu_state_t* state){
    thread_t* thread = get_current_thread();

    schedule_process_waiting(state);

    resource_queue_inque(disk_queue, thread, fdelete_proc);
}
