#define NULL 0
#define MEMORY_INIT_INT 0xC0

#include <stdint.h>

void minit();

char* malloc(uint32_t size);

void mfree(char* ptr, uint32_t size);