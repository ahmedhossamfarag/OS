#define MemoryBeginAddress 0xB100000
#define MemoryEnd 0x10000000 - 0x100
#define MemorySize MemoryEnd - MemoryBeginAddress
#define NULL 0

#include <stdint.h>

void memory_init();

char* alloc(uint64_t size);

char* alloc_align(uint64_t size, uint64_t align);

void free(char* ptr, uint64_t size);
