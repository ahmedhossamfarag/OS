#include "libc.h"
#include "resources.h"
#include "screen_print.h"

void print_str(const char* str){
    asm("mov %0, %%eax"::"i"(SCREEN));
    asm("int %0"::"i"(RESOURCE_REQUEST_INT));

    screen_print_str(str);
    
    asm("mov %0, %%eax" ::"i"(SCREEN));
    asm("int %0" ::"i"(RESOURCE_FREE_INT));
}
