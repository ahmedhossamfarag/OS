#define KERNEL_DATA_SEGMENT 0x10
#define KERNEL_STACK_SIZE 0X1000
#define KERNEK_STACK_POINTER(i) 0XB000000 - (i * KERNEL_STACK_SIZE)
#define TSS_INDEX(i) (5 + i)
#define TSS_SELECTOR(i) TSS_INDEX(i) * 8
#include <stdint.h>

typedef struct  {
    uint32_t reserved;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved_2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved_3;
    uint16_t reserved_4;
    uint16_t io_map_base;
} __attribute__((packed)) tss_entry_t;

void tss_init();

void enable_tss();