#define NULL 0

#include <stdint.h>

void memory_init();

char* alloc(uint64_t size);

char* alloc_align(uint64_t size, uint64_t align);

void free(char* ptr, uint64_t size);
