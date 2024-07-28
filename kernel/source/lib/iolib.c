#include "iolib.h"
#include "syscall_map.h"


FILE *fopen(const char *name)
{
    asm("mov %0, %%esi\n\t""int $0x80":: "i"(FOPEN_SYSCALL), "a"((uint32_t)name));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));

    return (FILE*)result;
}

uint8_t fsize(FILE *file, uint32_t *to)
{
    asm("mov %0, %%esi\n\t""int $0x80":: "i"(FSIZE_SYSCALL), "a"((uint32_t)file), "d"((uint32_t)to));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t fclose(FILE *file)
{
    asm("mov %0, %%esi\n\t""int $0x80":: "i"(FCLOSE_SYSCALL), "a"((uint32_t)file));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t fread(FILE *file, char *to, uint32_t seek, uint32_t count)
{
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(FREAD_SYSCALL), "a"((uint32_t)file),"d"((uint32_t)to),"b"(seek),"c"(count));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t fwrite(FILE *file, char *from, uint32_t count)
{
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(FWRITE_SYSCALL), "a"((uint32_t)file),"d"((uint32_t)from),"c"(count));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}

FILE* fcreate(FILE *parent, char *name, uint8_t is_dir)
{
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(FCREATE_SYSCALL), "a"((uint32_t)parent),"d"((uint32_t)name),"b"(is_dir));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (FILE*)result;
}

uint8_t fdelete(FILE *file)
{
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(FDELETE_SYSCALL), "a"((uint32_t)file));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t flist(FILE *dir, char *to)
{
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(FLIST_SYSCALL), "a"((uint32_t)dir),"d"((uint32_t)to));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t prints(const char *str)
{
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(PRINT_SYSCALL), "d"((uint32_t)str));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t scans(char *to, uint32_t count)
{
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(SCAN_SYSCALL), "d"((uint32_t)to),"c"(count));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t scans_line(char *to)
{
    asm("mov %0, %%esi\n\t""int $0x80"::"i"(SCAN_LINE_SYSCALL), "d"((uint32_t)to));

    uint32_t result;
    asm("mov %%eax, %0":"=m"(result));
    
    return (uint8_t)result;
}
