#include "acpi.h"
#include "strlib.h"
#include "info.h"

void *find_rsdp() {
    for (uintptr_t addr = RSDP_SEARCH_START; addr < RSDP_SEARCH_END; addr += RSDP_SEARCH_INCREMENT) {
        if (str_cmp_n((char *)addr, RSDP_SIGNATURE, 8) == 0) {
            return (void *)addr;
        }
    }
    return 0;
}


uint8_t acpi_checksum(void *table, uint32_t length) {
    uint8_t sum = 0;
    uint8_t *ptr = table;
    for (uint32_t i = 0; i < length; i++) {
        sum += ptr[i];
    }
    return sum;
}



void* find_sdt(acpi_sdt_header_t *rsdt, const char* signature) {
    int entry_count = (rsdt->length - sizeof(acpi_sdt_header_t)) / 4;
    uint32_t *entries = (uint32_t *)((uintptr_t)rsdt + sizeof(acpi_sdt_header_t));
    for (int i = 0; i < entry_count; i++) {
        acpi_sdt_header_t *header = (acpi_sdt_header_t *)(uintptr_t)entries[i];
        if(str_cmp_n(header->signature, signature, 4) == 0){
            return (void*) header;
        }
    }
    return 0;
}

void parse_madt(madt_t *madt) {
    uint8_t *ptr = madt->entries;
    uint8_t *end = (uint8_t *)madt + madt->header.length;

    while (ptr < end) {
        uint8_t type = ptr[0];
        uint8_t length = ptr[1];
        switch (type) {
            case 0: { // Processor Local APIC
                madt_apic_t *entry = (void *)ptr;
                info_add_apic_id(entry->apic_id);
                break;
            }
            default:
                break;
        }

        ptr += length;
    }
}

void acpi_init() {
    rsdp_descriptor_t *rsdp = find_rsdp();

    // if (!rsdp || acpi_checksum(rsdp, sizeof(rsdp_descriptor_t)) != 0) {
    if(!rsdp){
        // Handle error
        return;
    }

    acpi_sdt_header_t *rsdt = (acpi_sdt_header_t *)(uintptr_t)rsdp->rsdt_address;
    if (acpi_checksum(rsdt, rsdt->length) != 0) {
        // Handle error
        return;
    }

    madt_t* madt = find_sdt(rsdt, MADT_SIGNATURE);
    if(madt){
        parse_madt(madt);
    }
}
