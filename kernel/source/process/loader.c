#include "loader.h"
#include "pagging.h"
#include "scheduler.h"

extern void proc1();

void load_program()
{
    uint32_t* cr3 = get_available_pagging_dir();
    if(cr3){
        uint32_t* org = (uint32_t*) virtual_to_physical(0, cr3);

        uint32_t* src = (uint32_t*) &proc1;
        for (int i = 0; i < 10; i++)
        {
            *org = *src;
            org++;
            src++;
        }
        

        add_new_process(1, 0, (uint32_t)cr3, 0, 0xFFF, 0x100);
    }
}