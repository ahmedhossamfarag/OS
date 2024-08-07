#include <stdint.h>

uint8_t prints(const char *str)
{
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(0x10), "d"((uint32_t)str));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}
