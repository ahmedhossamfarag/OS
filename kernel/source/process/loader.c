#include "loader.h"
#include "pagging.h"
#include "scheduler.h"
#include "ata.h"

#define SHELL_OFFSET 201
#define SHELL_SIZE 50
#define SHELL_STACK_SZ 0x1000 

void load_program()
{
    uint32_t* cr3 = get_available_pagging_dir();
    if(cr3){
        uint32_t zero = virtual_to_physical(0, (uint32_t*) cr3);
        ata_read(PRIMARY_BASE, 0, SHELL_OFFSET, SHELL_SIZE, (uint32_t*)zero);
        uint32_t ebp = SHELL_SIZE*512 + SHELL_STACK_SZ;
        #define MEMO_PADDING 0x100
        add_new_process(1, 0, (uint32_t)cr3, 0, ebp, ebp + MEMO_PADDING);
    }
}