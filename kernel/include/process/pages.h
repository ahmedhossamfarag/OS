#include <stdint.h>

#define SEGMENT_SIZE 0x400000
#define SEGMENT_NO 1024

void pages_init();

uint32_t pages_alloc();

uint32_t pages_alloc_next(uint32_t seg);

void pages_free(uint32_t seg);

uint32_t pages_nfree_segments();