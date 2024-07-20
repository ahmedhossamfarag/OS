#define RSDP_SIGNATURE "RSD PTR "
#define RSDP_SEARCH_START 0x000E0000
#define RSDP_SEARCH_END 0x000FFFFF
#define RSDP_SEARCH_INCREMENT 16

#include <stdint.h>

typedef struct
{
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;       // Only if revision >= 2
    uint64_t xsdt_address; // Only if revision >= 2
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed)) rsdp_descriptor_t;

typedef struct
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed)) acpi_sdt_header_t;

void acpi_init();

#pragma region MADT

#define MADT_SIGNATURE "APIC"

typedef struct
{
    acpi_sdt_header_t header;
    uint32_t local_apic_address;
    uint32_t flags;
    uint8_t entries[];
} __attribute__((packed)) madt_t;

typedef struct
{
    uint8_t type;
    uint8_t length;
    uint8_t acpi_processor_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__((packed)) madt_apic_t;

#pragma endregion