#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

#pragma region PCI_OFFSETS

// Common Header Offsets
#define PCI_VENDOR_ID           0x00    // 2 bytes
#define PCI_DEVICE_ID           0x02    // 2 bytes
#define PCI_COMMAND             0x04    // 2 bytes
#define PCI_STATUS              0x06    // 2 bytes
#define PCI_REVISION_ID         0x08    // 1 byte
#define PCI_PROG_IF             0x09    // 1 byte
#define PCI_SUBCLASS            0x0A    // 1 byte
#define PCI_CLASS               0x0B    // 1 byte
#define PCI_CACHE_LINE_SIZE     0x0C    // 1 byte
#define PCI_LATENCY_TIMER       0x0D    // 1 byte
#define PCI_HEADER_TYPE         0x0E    // 1 byte
#define PCI_BIST                0x0F    // 1 byte

// Header Type 0x00 (Standard Device)
#define PCI_BAR0                0x10    // 4 bytes
#define PCI_BAR1                0x14    // 4 bytes
#define PCI_BAR2                0x18    // 4 bytes
#define PCI_BAR3                0x1C    // 4 bytes
#define PCI_BAR4                0x20    // 4 bytes
#define PCI_BAR5                0x24    // 4 bytes
#define PCI_CARDBUS_CIS         0x28    // 4 bytes
#define PCI_SUBSYSTEM_VENDOR_ID 0x2C    // 2 bytes
#define PCI_SUBSYSTEM_ID        0x2E    // 2 bytes
#define PCI_EXPANSION_ROM_BASE  0x30    // 4 bytes
#define PCI_CAPABILITY_LIST     0x34    // 1 byte
#define PCI_INTERRUPT_LINE      0x3C    // 1 byte
#define PCI_INTERRUPT_PIN       0x3D    // 1 byte
#define PCI_MIN_GRANT           0x3E    // 1 byte
#define PCI_MAX_LATENCY         0x3F    // 1 byte

// Header Type 0x01 (PCI-to-PCI Bridge)
#define PCI_PRIMARY_BUS         0x18    // 1 byte
#define PCI_SECONDARY_BUS       0x19    // 1 byte
#define PCI_SUBORDINATE_BUS     0x1A    // 1 byte
#define PCI_SECONDARY_LATENCY   0x1B    // 1 byte
#define PCI_IO_BASE             0x1C    // 1 byte
#define PCI_IO_LIMIT            0x1D    // 1 byte
#define PCI_SECONDARY_STATUS    0x1E    // 2 bytes
#define PCI_MEMORY_BASE         0x20    // 2 bytes
#define PCI_MEMORY_LIMIT        0x22    // 2 bytes
#define PCI_PREFETCH_MEMORY_BASE 0x24   // 2 bytes
#define PCI_PREFETCH_MEMORY_LIMIT 0x26  // 2 bytes
#define PCI_PREFETCH_BASE_UPPER 0x28    // 4 bytes
#define PCI_PREFETCH_LIMIT_UPPER 0x2C   // 4 bytes
#define PCI_IO_BASE_UPPER       0x30    // 2 bytes
#define PCI_IO_LIMIT_UPPER      0x32    // 2 bytes
#define PCI_BRIDGE_EXPANSION_ROM_BASE 0x38 // 4 bytes
#define PCI_BRIDGE_INTERRUPT_LINE 0x3C // 1 byte
#define PCI_BRIDGE_INTERRUPT_PIN 0x3D  // 1 byte
#define PCI_BRIDGE_CONTROL      0x3E    // 2 bytes

// Header Type 0x02 (CardBus Bridge)
#define PCI_CB_CAPABILITY_LIST  0x14    // 1 byte
#define PCI_CB_SECONDARY_STATUS 0x16    // 2 bytes
#define PCI_CB_BUS_NUMBER       0x18    // 1 byte
#define PCI_CB_CARD_BUS_NUMBER  0x19    // 1 byte
#define PCI_CB_SUBORDINATE_BUS  0x1A    // 1 byte
#define PCI_CB_LATENCY_TIMER    0x1B    // 1 byte
#define PCI_CB_MEMORY_BASE_0    0x1C    // 4 bytes
#define PCI_CB_MEMORY_LIMIT_0   0x20    // 4 bytes
#define PCI_CB_MEMORY_BASE_1    0x24    // 4 bytes
#define PCI_CB_MEMORY_LIMIT_1   0x28    // 4 bytes
#define PCI_CB_IO_BASE_0        0x2C    // 4 bytes
#define PCI_CB_IO_LIMIT_0       0x30    // 4 bytes
#define PCI_CB_IO_BASE_1        0x34    // 4 bytes
#define PCI_CB_IO_LIMIT_1       0x38    // 4 bytes
#define PCI_CB_INTERRUPT_LINE   0x3C    // 1 byte
#define PCI_CB_INTERRUPT_PIN    0x3D    // 1 byte
#define PCI_CB_BRIDGE_CONTROL   0x3E    // 2 bytes
#define PCI_CB_SUBSYSTEM_VENDOR_ID 0x40 // 2 bytes
#define PCI_CB_SUBSYSTEM_ID     0x42    // 2 bytes
#define PCI_CB_legacy_mode_base 0x44    // 4 bytes

#pragma endregion

#pragma region PCI_CLASSES

// PCI Class Codes
#define PCI_CLASS_PRE_20               0x00
#define PCI_CLASS_MASS_STORAGE         0x01
#define PCI_CLASS_NETWORK              0x02
#define PCI_CLASS_DISPLAY              0x03
#define PCI_CLASS_MULTIMEDIA           0x04
#define PCI_CLASS_MEMORY               0x05
#define PCI_CLASS_BRIDGE               0x06
#define PCI_CLASS_COMMUNICATION        0x07
#define PCI_CLASS_SYSTEM               0x08
#define PCI_CLASS_INPUT                0x09
#define PCI_CLASS_DOCKING              0x0A
#define PCI_CLASS_PROCESSOR            0x0B
#define PCI_CLASS_SERIAL_BUS           0x0C
#define PCI_CLASS_WIRELESS             0x0D
#define PCI_CLASS_INTELLIGENT_IO       0x0E
#define PCI_CLASS_SATELLITE            0x0F
#define PCI_CLASS_CRYPT                0x10
#define PCI_CLASS_SIGNAL_PROCESSING    0x11
#define PCI_CLASS_UNDEFINED            0xFF

// PCI Subclass Codes for each Class
// Pre-2.0 Devices
#define PCI_SUBCLASS_PRE_20_NON_VGA    0x00
#define PCI_SUBCLASS_PRE_20_VGA        0x01

// Mass Storage Controller
#define PCI_SUBCLASS_SCSI              0x00
#define PCI_SUBCLASS_IDE               0x01
#define PCI_SUBCLASS_FLOPPY            0x02
#define PCI_SUBCLASS_IPI               0x03
#define PCI_SUBCLASS_RAID              0x04
#define PCI_SUBCLASS_ATA               0x05
#define PCI_SUBCLASS_SATA              0x06
#define PCI_SUBCLASS_SAS               0x07
#define PCI_SUBCLASS_OTHER_MASS        0x80

// Network Controller
#define PCI_SUBCLASS_ETHERNET          0x00
#define PCI_SUBCLASS_TOKEN_RING        0x01
#define PCI_SUBCLASS_FDDI              0x02
#define PCI_SUBCLASS_ATM               0x03
#define PCI_SUBCLASS_ISDN              0x04
#define PCI_SUBCLASS_WORLDFIP          0x05
#define PCI_SUBCLASS_PICMG             0x06
#define PCI_SUBCLASS_OTHER_NETWORK     0x80

// Display Controller
#define PCI_SUBCLASS_VGA               0x00
#define PCI_SUBCLASS_XGA               0x01
#define PCI_SUBCLASS_3D                0x02
#define PCI_SUBCLASS_OTHER_DISPLAY     0x80

// Multimedia Controller
#define PCI_SUBCLASS_VIDEO_CNTR        0x00
#define PCI_SUBCLASS_AUDIO_CNTR        0x01
#define PCI_SUBCLASS_TELEPHONY         0x02
#define PCI_SUBCLASS_AUDIO_DEVICE      0x03
#define PCI_SUBCLASS_OTHER_MULTIMEDIA  0x80

// Memory Controller
#define PCI_SUBCLASS_RAM               0x00
#define PCI_SUBCLASS_FLASH             0x01
#define PCI_SUBCLASS_OTHER_MEMORY      0x80

// Bridge Device
#define PCI_SUBCLASS_HOST              0x00
#define PCI_SUBCLASS_ISA               0x01
#define PCI_SUBCLASS_EISA              0x02
#define PCI_SUBCLASS_MCA               0x03
#define PCI_SUBCLASS_PCI2PCI           0x04
#define PCI_SUBCLASS_PCMCIA            0x05
#define PCI_SUBCLASS_NUBUS             0x06
#define PCI_SUBCLASS_CARDBUS           0x07
#define PCI_SUBCLASS_RACEWAY           0x08
#define PCI_SUBCLASS_SEMI_TRANSPARENT  0x09
#define PCI_SUBCLASS_BRIDGE_INFINIBAND 0x0A
#define PCI_SUBCLASS_OTHER_BRIDGE      0x80

// Communication Controller
#define PCI_SUBCLASS_SERIAL            0x00
#define PCI_SUBCLASS_PARALLEL          0x01
#define PCI_SUBCLASS_MULTI_PORT        0x02
#define PCI_SUBCLASS_MODEM             0x03
#define PCI_SUBCLASS_GPIB              0x04
#define PCI_SUBCLASS_SMART_CARD        0x05
#define PCI_SUBCLASS_OTHER_COMM        0x80

// System Peripheral
#define PCI_SUBCLASS_PIC               0x00
#define PCI_SUBCLASS_DMA               0x01
#define PCI_SUBCLASS_TIMER             0x02
#define PCI_SUBCLASS_RTC               0x03
#define PCI_SUBCLASS_PCI_HOTPLUG       0x04
#define PCI_SUBCLASS_SD_HOST_CONTROLLER 0x05
#define PCI_SUBCLASS_OTHER_SYSTEM      0x80

// Input Device
#define PCI_SUBCLASS_KEYBOARD          0x00
#define PCI_SUBCLASS_PEN               0x01
#define PCI_SUBCLASS_MOUSE             0x02
#define PCI_SUBCLASS_SCANNER           0x03
#define PCI_SUBCLASS_GAMEPORT          0x04
#define PCI_SUBCLASS_OTHER_INPUT       0x80

// Docking Station
#define PCI_SUBCLASS_GENERIC_DOCKING   0x00
#define PCI_SUBCLASS_OTHER_DOCKING     0x80

// Processor
#define PCI_SUBCLASS_386               0x00
#define PCI_SUBCLASS_486               0x01
#define PCI_SUBCLASS_PENTIUM           0x02
#define PCI_SUBCLASS_ALPHA             0x10
#define PCI_SUBCLASS_POWERPC           0x20
#define PCI_SUBCLASS_MIPS              0x30
#define PCI_SUBCLASS_CO_PROCESSOR      0x40

// Serial Bus Controller
#define PCI_SUBCLASS_FIREWIRE          0x00
#define PCI_SUBCLASS_ACCESS            0x01
#define PCI_SUBCLASS_SSA               0x02
#define PCI_SUBCLASS_USB               0x03
#define PCI_SUBCLASS_FIBER_CHANNEL     0x04
#define PCI_SUBCLASS_SMBUS             0x05
#define PCI_SUBCLASS_BUS_INFINIBAND    0x06
#define PCI_SUBCLASS_IPMI              0x07
#define PCI_SUBCLASS_SERCOS            0x08
#define PCI_SUBCLASS_CANBUS            0x09
#define PCI_SUBCLASS_OTHER_SERIAL      0x80

// Wireless Controller
#define PCI_SUBCLASS_IRDA              0x00
#define PCI_SUBCLASS_CONSUMER_IR       0x01
#define PCI_SUBCLASS_RF_CONTROLLER     0x10
#define PCI_SUBCLASS_BLUETOOTH         0x11
#define PCI_SUBCLASS_BROADBAND         0x12
#define PCI_SUBCLASS_ETHERNET_A        0x20
#define PCI_SUBCLASS_ETHERNET_B        0x21
#define PCI_SUBCLASS_OTHER_WIRELESS    0x80

// Intelligent I/O Controller
#define PCI_SUBCLASS_I2O               0x00

// Satellite Communication Controller
#define PCI_SUBCLASS_TV                0x01
#define PCI_SUBCLASS_AUDIO             0x02
#define PCI_SUBCLASS_VOICE             0x03
#define PCI_SUBCLASS_DATA              0x04

// Encryption/Decryption Controller
#define PCI_SUBCLASS_NET               0x00
#define PCI_SUBCLASS_ENTERTAINMENT     0x01
#define PCI_SUBCLASS_OTHER_CRYPT       0x80

// Signal Processing Controller
#define PCI_SUBCLASS_DPIO              0x00
#define PCI_SUBCLASS_OTHER_SIGNAL      0x80

#pragma endregion

#include <stdint.h>

void pci_init();