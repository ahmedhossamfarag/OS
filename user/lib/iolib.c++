#include "iolib.h"
#include "syscall_map.h"


FILE *fopen(const char *name, FILE* parent)
{
    asm("mov %0, %%rsi\n\t""int $0x80":: "i"(FOPEN_SYSCALL), "a"((uint64_t)name), "b"((uint64_t)parent));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));

    return (FILE*)result;
}

uint8_t fsize(FILE *file, uint32_t *to)
{
    asm("mov %0, %%rsi\n\t""int $0x80":: "i"(FSIZE_SYSCALL), "a"((uint64_t)file), "d"((uint64_t)to));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t fclose(FILE *file)
{
    asm("mov %0, %%rsi\n\t""int $0x80":: "i"(FCLOSE_SYSCALL), "a"((uint64_t)file));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t fread(FILE *file, char *to, uint32_t seek, uint32_t count)
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(FREAD_SYSCALL), "a"((uint64_t)file),"d"((uint64_t)to),"b"(seek),"c"(count));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t fwrite(FILE *file, char *from, uint32_t count)
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(FWRITE_SYSCALL), "a"((uint64_t)file),"d"((uint64_t)from),"c"(count));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

FILE* fcreate(FILE *parent, char *name, uint8_t is_dir)
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(FCREATE_SYSCALL), "a"((uint64_t)parent),"d"((uint64_t)name),"b"(is_dir));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (FILE*)result;
}

uint8_t fdelete(FILE *file)
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(FDELETE_SYSCALL), "a"((uint64_t)file));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t flist(FILE *dir, char *to)
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(FLIST_SYSCALL), "a"((uint64_t)dir),"d"((uint64_t)to));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t prints(const char *str)
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(PRINT_SYSCALL), "d"((uint64_t)str));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}

uint8_t scans(char *to, uint32_t count)
{
    asm("mov %0, %%rsi\n\t""int $0x80"::"i"(SCAN_SYSCALL), "d"((uint64_t)to),"c"(count));

    uint64_t result;
    asm("mov %%rax, %0":"=m"(result));
    
    return (uint8_t)result;
}
