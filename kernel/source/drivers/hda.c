#include "hda.h"
#include "vga_print.h"
#include "strlib.h"

extern uint16_t hda_vendorId;
extern uint16_t hda_deviceId;
extern uint32_t hda_memory_bar;
extern uint8_t hda_irq;

static uint32_t x;
static uint32_t y;
static uint32_t z;

void hda_write_byte(uint32_t reg, uint8_t value)
{
    *((volatile uint8_t *)(hda_memory_bar + reg)) = value;
}

uint8_t hda_read_byte(uint32_t reg)
{
    return *((volatile uint8_t *)(hda_memory_bar + reg));
}

void hda_write_word(uint32_t reg, uint16_t value)
{
    *((volatile uint16_t *)(hda_memory_bar + reg)) = value;
}

uint16_t hda_read_word(uint32_t reg)
{
    return *((volatile uint16_t *)(hda_memory_bar + reg));
}

void hda_write_dword(uint32_t reg, uint32_t value)
{
    *((volatile uint32_t *)(hda_memory_bar + reg)) = value;
}

uint32_t hda_read_dword(uint32_t reg)
{
    return *((volatile uint32_t *)(hda_memory_bar + reg));
}

static void hda_get_xyz(uint16_t gcap){
    x = 0x80 + HDA_ISS(gcap) * 0x20;
    y = x + HDA_OSS(gcap) * 0x20;
    z = y + HDA_BSS(gcap) * 0x20;
}

#define wait(x) for (int i = 0; i < x; i++)

#include "pic.h"
#include "interrupt.h"

void handler(){
    print("hda ")
    println(sxint(hda_read_dword(HDA_INTSTS)))
    
    pic_sendEOI(hda_irq);
    asm("sti");
}

void hda_init()
{
    idt_set_entry(PIC_M_OFFSET + hda_irq, (uint32_t)handler);
    irq_clear_mask(hda_irq);

    uint16_t gcap = hda_read_word(HDA_GCAP);
    hda_get_xyz(gcap);

    // reset
    hda_write_dword(HDA_GCTL, HDA_GCTL_CRST);
    do{
        wait(1000);
    } while (!(hda_read_dword(HDA_GCTL) & HDA_GCTL_CRST));

    // wake enabled
    hda_write_word(HDA_WAKEEN, 0xFFFF);

    // interrupts
    hda_write_dword(HDA_INTCTL, 0x3FFFFFFF);    
}