#include "ata.h"
#include "low_level.h"
#include "pic.h"
#include "apic.h"

static void ata_select_drive(uint16_t base, uint8_t drive) {
    outb(base + 6, 0xA0 | (drive << 4)); // 0xA0 for master, 0xB0 for slave
}

static void ata_wait_busy(uint16_t base) {
    while (inb(base + 7) & 0x80); // Wait for BSY flag to clear
}

static void ata_wait_drq(uint16_t base) {
    while (!(inb(base + 7) & 0x08)); // Wait for DRQ (data request) flag to set
}

void ata_read(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, void *buffer) {
    if(drive != 0 && drive != 1) return;

    if(base != PRIMARY_BASE && base != SECONDARY_BASE) return;

    uint16_t ctrl = base == PRIMARY_BASE ? PRIMARY_CTRL : SECONDARY_CTRL;
    
    ata_select_drive(base, drive);

    outb(ctrl, 0); // Disable interrupts

    outb(base + 2, sector_count); // Sector count
    outb(base + 3, lba & 0xFF); // LBA low
    outb(base + 4, (lba >> 8) & 0xFF); // LBA mid
    outb(base + 5, (lba >> 16) & 0xFF); // LBA high
    outb(base + 6, 0xE0 | (drive << 4) | ((lba >> 24) & 0x0F)); // Drive/Head
    outb(base + 7, ATA_READ); // Command

    for (int i = 0; i < sector_count; i++) {
        ata_wait_busy(base);
        ata_wait_drq(base);
        insw(base, buffer + i * 512, 256); // Read 256 words (512 bytes)
    }
}

void ata_write(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, const void *buffer) {
    if(drive != 0 && drive != 1) return;

    if(base != PRIMARY_BASE && base != SECONDARY_BASE) return;

    uint16_t ctrl = base == PRIMARY_BASE ? PRIMARY_CTRL : SECONDARY_CTRL;
   
    
    ata_select_drive(base, drive);

    outb(ctrl, 0); // Disable interrupts

    outb(base + 2, sector_count); // Sector count
    outb(base + 3, lba & 0xFF); // LBA low
    outb(base + 4, (lba >> 8) & 0xFF); // LBA mid
    outb(base + 5, (lba >> 16) & 0xFF); // LBA high
    outb(base + 6, 0xE0 | (drive << 4) | ((lba >> 24) & 0x0F)); // Drive/Head
    outb(base + 7, ATA_WRITE); // Command

    for (int i = 0; i < sector_count; i++) {
        ata_wait_busy(base);
        ata_wait_drq(base);
        outsw(base, buffer + i * 512, 256); // Write 256 words (512 bytes)
    }
}

uint8_t ata_get_status(){
    return inb(PRIMARY_BASE + 7);
}

void (*ata_handler_proc)(uint8_t);

void set_ata_handler_proc(void (*proc)(uint8_t))
{
    ata_handler_proc = proc;
}

void ata_handler()
{
    if(ata_handler_proc){
        ata_handler_proc(ata_get_status());
    }
    pic_sendEOI(14);
}


void apic_ata_handler()
{
    if(ata_handler_proc){
        ata_handler_proc(ata_get_status());
    }
    apic_sendEOI();
}
