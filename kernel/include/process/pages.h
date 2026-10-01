#include <stdint.h>

#define SEGMENT_SIZE 0x200000   // Corresponds to 2MB of address space with 4KB pages
#define SEGMENT_NO (0x100000000 / SEGMENT_SIZE)  // Total number of segments in 4GB address space

void pages_init();

uint64_t pages_alloc();

uint64_t pages_alloc_next(uint64_t seg);

void pages_free(uint64_t seg);

uint32_t pages_nfree_segments();