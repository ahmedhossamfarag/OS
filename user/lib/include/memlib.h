#define NULL 0

#include <stdint.h>

extern "C" void minit();

void* malloc(uint64_t size);

void mfree(void* ptr, uint64_t size);