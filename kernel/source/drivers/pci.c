#include "pci.h"
#include "low_level.h"
#include "ethernet.h"
#include "hda.h"

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

uint16_t hda_vendorId;
uint16_t hda_deviceId;
uint32_t hda_memory_bar;
uint8_t hda_irq;

void pci_hda_device_init(uint8_t bus, uint8_t device, uint8_t function)
{
    hda_vendorId = pci_read_config(bus, device, function, PCI_VENDOR_ID);
    hda_deviceId = pci_read_config(bus, device, function, PCI_DEVICE_ID);

    uint32_t bar_value = pci_read_config(bus, device, function, PCI_BAR0);
    hda_memory_bar = bar_value & ~0x0F;

    hda_irq = pci_read_config(bus, device, function, PCI_INTERRUPT_LINE);
}

uint16_t ethernet_vendorId;
uint16_t ethernet_deviseId;
uint32_t ethernet_memory_bar;
uint8_t ethernet_irq;

void pci_ethernet_device_init(uint8_t bus, uint8_t device, uint8_t function)
{
    ethernet_vendorId = pci_read_config(bus, device, function, PCI_VENDOR_ID);
    ethernet_deviseId = pci_read_config(bus, device, function, PCI_DEVICE_ID);

    uint32_t bar_value = pci_read_config(bus, device, function, PCI_BAR0);
    ethernet_memory_bar = bar_value & ~0x0F;

    
    ethernet_irq = pci_read_config(bus, device, function, PCI_INTERRUPT_LINE);
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
                        pci_hda_device_init(bus, device, function);
                    }
                    if (base_class == PCI_CLASS_NETWORK && sub_class == PCI_SUBCLASS_ETHERNET)
                    {
                        pci_ethernet_device_init(bus, device, function);
                    }
                }
            }
        }
    }
    // ethernet_init();
    hda_init();
}