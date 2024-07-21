#define MEMORY_N_MAP 0x8000
#define MEMORY_MAP 0x8004
#define DISk_SIZE 0x8400
#define PROCESSOR_NO 0x8500

#define MAX_N_APIC_IDS 10

#include <stdint.h>

typedef struct  {
    uint64_t baseAddr;
    uint64_t length;
    uint32_t type;
    uint32_t acpi;
}memory_region_t;

void info_init();

uint32_t info_get_processor_no();

uint32_t info_get_memory_size();

void info_add_apic_id(uint32_t id);

uint32_t info_get_apic_id(uint8_t i);
