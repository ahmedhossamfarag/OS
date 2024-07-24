#define MemoryBeginAddress 0xB000000 + 0x100
#define MemorySize 0x1000000 - 0x200
#define MemoryEnd MemoryBeginAddress + MemorySize 
#define NULL 0

#include <stdint.h>

void memory_init();

char* alloc(uint32_t size);

void free(char* ptr, uint32_t size);
