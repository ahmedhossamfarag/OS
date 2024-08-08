#define MemoryBeginAddress 0xB900000
#define MemoryEnd 0x10000000 - 0x100
#define MemorySize MemoryEnd - MemoryBeginAddress
#define NULL 0

#include <stdint.h>

void memory_init();

char* alloc(uint32_t size);

char* alloc_align(uint32_t size, uint32_t align);

void free(char* ptr, uint32_t size);
