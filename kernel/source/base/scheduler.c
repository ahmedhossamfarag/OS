#include "scheduler.h"
#include "screen_print.h"
#include "strlib.h"
void add_process(pcb_t *process)
{
}

pcb_t *remove_process()
{
    return 0;
}

void schudler_init()
{
    set_timer_handler_proc(schudle);
    enable_timer(10);
}

void schudle(cpu_state_t* state)
{
    char s[10];
    print(int_to_hex_str(state->esp, s));
}

void context_switch(pcb_t *next_process)
{
}
