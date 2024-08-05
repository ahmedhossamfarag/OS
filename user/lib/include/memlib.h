#define NULL 0

#include <stdint.h>

extern "C" void minit();

void* malloc(uint32_t size);

void mfree(void* ptr, uint32_t size);