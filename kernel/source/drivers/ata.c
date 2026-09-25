#include "ata.h"
#include "low_level.h"
#include "pic.h"
#include "apic.h"
#include "vga_print.h"
#include "strlib.h"

#define ata_busy(base) (inb(base + 7) & ATA_STATUS_BSY)
#define ata_drq(base) (inb(base + 7) & ATA_STATUS_DRQ)
#define ata_error(base)  (inb(base + 7) & ATA_STATUS_ERR)

static void ata_select_drive(uint16_t base, uint8_t drive) {
    outb(base + 6, 0xA0 | (drive << 4)); // 0xA0 for master, 0xB0 for slave
}

static void ata_print_status(uint16_t base) {
    char str[20];
    print("Status: ");
    print(int_to_hex_str(inb(base + 7), str));
    println("");
    print("Error: ");
    print(int_to_hex_str(inb(base + 1), str));
    println("");
}

static void ata_small_delay(uint16_t base) {
    inb(base + 356);
    inb(base + 356);
    inb(base + 356);
    inb(base + 356);
}

static uint8_t ata_wait_busy(uint16_t base) {
    uint32_t timeout = ATA_WAIT_TIMEOUT;
    while (timeout > 0) {
        if (!(inb(base + 7) & 0x80)) {
            return 1; // BSY flag cleared successfully
        }
        timeout--;
    }
    print("ATA wait busy timeout\n");
    ata_print_status(base);
    return 0; // Timeout reached while waiting for BSY to clear
}

static uint8_t ata_wait_drq(uint16_t base) {
    uint32_t timeout = ATA_WAIT_TIMEOUT;
    while (timeout > 0) {
        if (inb(base + 7) & 0x08) {
            return 1; // DRQ flag set successfully
        }
        timeout--;
    }
    print("ATA wait drq timeout\n");
    ata_print_status(base);
    return 0; // Timeout reached while waiting for DRQ to set
}


void ata_init() {
    outb(PRIMARY_CTRL, 0x04);
    // Small delay
    outb(PRIMARY_CTRL, 0x00);
}

static void ata_read_request(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count){
    //  uint16_t ctrl = base == PRIMARY_BASE ? PRIMARY_CTRL : SECONDARY_CTRL;
    
    ata_select_drive(base, drive);

    // outb(ctrl, 0); // Disable interrupts

    outb(base + 2, sector_count); // Sector count
    outb(base + 3, lba & 0xFF); // LBA low
    outb(base + 4, (lba >> 8) & 0xFF); // LBA mid
    outb(base + 5, (lba >> 16) & 0xFF); // LBA high
    outb(base + 6, 0xE0 | (drive << 4) | ((lba >> 24) & 0x0F)); // Drive/Head
    outb(base + 7, ATA_READ); // Command
}

uint8_t ata_read(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, void *buffer) {
    if(drive != 0 && drive != 1) return 0;

    if(base != PRIMARY_BASE && base != SECONDARY_BASE) return 0;

    ata_read_request(base, drive, lba, sector_count);
    
    // Small delay
    ata_small_delay(base);

    for (int i = 0; i < sector_count; i++) {
        if(!ata_wait_busy(base)) return 0;
        if(!ata_wait_drq(base)) return 0;
        insw(base, buffer + i * 512, 256); // Read 256 words (512 bytes)
    }

    return 1;
}

static void ata_write_request(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count){
    // uint16_t ctrl = base == PRIMARY_BASE ? PRIMARY_CTRL : SECONDARY_CTRL;
   
    
    ata_select_drive(base, drive);

    // outb(ctrl, 0); // Disable interrupts

    outb(base + 2, sector_count); // Sector count
    outb(base + 3, lba & 0xFF); // LBA low
    outb(base + 4, (lba >> 8) & 0xFF); // LBA mid
    outb(base + 5, (lba >> 16) & 0xFF); // LBA high
    outb(base + 6, 0xE0 | (drive << 4) | ((lba >> 24) & 0x0F)); // Drive/Head
    outb(base + 7, ATA_WRITE); // Command
}

uint8_t ata_write(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, const void *buffer) {
    if(drive != 0 && drive != 1) return 0;

    if(base != PRIMARY_BASE && base != SECONDARY_BASE) return 0;

    ata_write_request(base, drive, lba, sector_count);

    // Small delay
    ata_small_delay(base);

    for (int i = 0; i < sector_count; i++) {
        if(!ata_wait_busy(base)) return 0;
        if(!ata_wait_drq(base)) return 0;
        outsw(base, buffer + i * 512, 256); // Write 256 words (512 bytes)
        ata_wait_busy(base);
    }

    return 1;
}

static struct{
    uint8_t rw;
    uint16_t base;
    uint8_t drive;
    uint32_t lba;
    uint8_t sector_count;
    void *buffer;
    void (*success_proc)();
    void (*error_proc)();
} ata_args;


void ata_read_sync(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, void *buffer, void (*success_proc)(), void (*error_proc)()) {
    if(drive != 0 && drive != 1) return;

    if(base != PRIMARY_BASE && base != SECONDARY_BASE) return;

    ata_args.rw = ATA_READ;
    ata_args.base = base;
    ata_args.drive = drive;
    ata_args.lba = lba;
    ata_args.sector_count = sector_count;
    ata_args.buffer = buffer;
    ata_args.success_proc = success_proc;
    ata_args.error_proc = error_proc;

    ata_read_request(base, drive, lba, sector_count);
}


void ata_write_sync(uint16_t base, uint8_t drive, uint32_t lba, uint8_t sector_count, void *buffer, void (*success_proc)(), void (*error_proc)()) {
    if(drive != 0 && drive != 1) return;

    if(base != PRIMARY_BASE && base != SECONDARY_BASE) return;

    ata_args.rw = ATA_WRITE;
    ata_args.base = base;
    ata_args.drive = drive;
    ata_args.lba = lba;
    ata_args.sector_count = sector_count;
    ata_args.buffer = buffer;
    ata_args.success_proc = success_proc;
    ata_args.error_proc = error_proc;


    ata_write_request(base, drive, lba, sector_count);

    outsw(base, buffer, 256); // Write 256 words (512 bytes)

}


static void ata_handler_proc()
{
    if(ata_args.sector_count){
        if(ata_error(ata_args.base)){
            if(ata_args.error_proc){
                ata_args.error_proc();
            }
            return;
        }
        if(ata_args.rw == ATA_READ){
            if(!ata_busy(ata_args.base) && ata_drq(ata_args.base)){
                insw(ata_args.base, ata_args.buffer, 256); // Read 256 words (512 bytes)
                ata_args.sector_count --;
                ata_args.buffer += 512;
                if(!ata_args.sector_count){
                    if(ata_args.success_proc){
                        ata_args.success_proc();
                    }
                }
            }
        }else if(ata_args.rw == ATA_WRITE){
            ata_args.sector_count --;
            ata_args.buffer += 512;
            if(!ata_args.sector_count){
                if(ata_args.success_proc){
                    ata_args.success_proc();
                }
            }else{
                if(!ata_busy(ata_args.base) && ata_drq(ata_args.base)){
                    outsw(ata_args.base, ata_args.buffer, 256); // Write 256 words (512 bytes)
                }
            }
        }
    }
}


void ata_handler()
{
    ata_handler_proc();
    pic_sendEOI(14);
}


void apic_ata_handler()
{
    ata_handler_proc();
    apic_sendEOI();
}


uint8_t ata_identify_drive(uint16_t base, uint8_t drive,  void *buffer){
    if(drive != 0 && drive != 1) return 0;

    if(base != PRIMARY_BASE && base != SECONDARY_BASE) return 0;

    ata_select_drive(base, drive);

    outb(base + 7, ATA_IDENTIFY_DRIVE); // Command

    ata_small_delay(base); // Small delay

    if(!ata_wait_busy(base)) return 0;
    if(!ata_wait_drq(base)) return 0;

    insw(base, buffer, 256); // Read 256 words (512 bytes)

    return 1;
}