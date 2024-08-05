#include "loader.h"
#include "pagging.h"
#include "scheduler.h"
#include "ata.h"
#include "memory.h"
#include "elf.h"
#include "math.h"

#define SHELL_OFFSET 201
#define SHELL_SIZE 50
#define SHELL_STACK_SZ 0x1000 

void load_program()
{
    uint32_t* cr3 = get_available_pagging_dir();
    if(cr3){
        char* file = alloc(SHELL_SIZE * 512);
        ata_read(PRIMARY_BASE, 0, SHELL_OFFSET, SHELL_SIZE, file);

        uint32_t current_cr3;
        asm("mov %%cr3, %0":"=r"(current_cr3));
        uint32_t th_cr3 = (uint32_t) cr3;
        asm volatile("mov %0, %%cr3" :: "r"(th_cr3));

        uint32_t eip;
        uint32_t textend = elf_load_file(file, &eip);
    
        asm volatile("mov %0, %%cr3" :: "r"(current_cr3));
        if(textend){
            uint32_t ebp = math_cielm(textend, 512) + SHELL_STACK_SZ;
            #define MEMO_PADDING 0x100
            add_new_process(1, 0, (uint32_t)cr3, eip, ebp, ebp + MEMO_PADDING);
        }else{
            free_pagging_dir(cr3);
        }
    }
}