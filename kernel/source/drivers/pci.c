#include "pci.h"
#include "low_level.h"

uint32_t pci_config_address(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    return (uint32_t)((bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
}

uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    uint8_t shift[4] = {0x0, 0x8, 0x10, 0x18};
    outl(PCI_CONFIG_ADDRESS, pci_config_address(bus, device, function, offset));
    return inl(PCI_CONFIG_DATA) >> shift[offset & 0x3];
}

static uint32_t audio_io_bar;
static uint32_t audio_memory_bar;

void pci_audio_device_init(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t bar_value = pci_read_config(bus, device, function, PCI_BAR0);
    if (bar_value & 0x01)
    {
        audio_io_bar = bar_value & ~0x03;
    }
    else
    {
        audio_memory_bar = bar_value & ~0x0F;
    }
}

static uint32_t ethernet_io_bar;
static uint32_t ethernet_memory_bar;

void pci_ethernet_device_init(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t bar_value = pci_read_config(bus, device, function, PCI_BAR0);
    if (bar_value & 0x01)
    {
        ethernet_io_bar = bar_value & ~0x03;
    }
    else
    {
        ethernet_memory_bar = bar_value & ~0x0F;
    }
}

void pci_init()
{
    for (int bus = 0; bus < 256; bus++)
    {
        for (int device = 0; device < 32; device++)
        {
            for (int function = 0; function < 8; function++)
            {
                uint16_t vendorID = pci_read_config(bus, device, function, PCI_VENDOR_ID);
                if (vendorID != 0xFFFF)
                {
                    uint16_t class_code = pci_read_config(bus, device, function, PCI_SUBCLASS);
                    uint8_t base_class = (class_code >> 8) & 0xFF;
                    uint8_t sub_class = class_code & 0xFF;
                    if (base_class == PCI_CLASS_MULTIMEDIA && sub_class == PCI_SUBCLASS_AUDIO_DEVICE)
                    {
                        pci_audio_device_init(bus, device, function);
                    }
                    if (base_class == PCI_CLASS_NETWORK && sub_class == PCI_SUBCLASS_ETHERNET)
                    {
                        pci_ethernet_device_init(bus, device, function);
                    }
                }
            }
        }
    }
}