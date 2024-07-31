#include "hda.h"

extern uint16_t hda_vendorId;
extern uint16_t hda_deviceId;
extern uint32_t hda_memory_bar;
extern uint8_t hda_irq;

void hda_write_reg(uint32_t reg, uint32_t value)
{
    *((volatile uint32_t *)(hda_memory_bar + reg)) = value;
}

uint32_t hda_read_reg(uint32_t reg)
{
    return *((volatile uint32_t *)(hda_memory_bar + reg));
}

void hda_init()
{

}