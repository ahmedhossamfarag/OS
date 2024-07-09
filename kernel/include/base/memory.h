#define MemoryBeginAddress 0xC00000
#define MemorySize 0x100000
#define MemoryEnd MemoryBeginAddress + MemorySize 
#define NULL 0

#include <stdint.h>

void memory_init();

char* alloc(uint32_t size);

void free(char* ptr, uint32_t size);
