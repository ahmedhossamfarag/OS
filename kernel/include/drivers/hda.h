#pragma region HDA

// Global Registers
#define HDA_GCTL      0x08  // Global Control // 4 bytes
#define HDA_WAKEEN    0x0C  // Wake Enable // 2 bytes
#define HDA_WAKESTS   0x0E  // Wake Status // 2 bytes
#define HDA_STATESTS  0x0E  // State Change Status
#define HDA_GSTS      0x10  // Global Status

#define HDA_GCTL_CRST 0x00000001

// Interrupt Registers
#define HDA_INTCTL    0x20  // Interrupt Control
#define HDA_INTSTS    0x24  // Interrupt Status

// Codec Communication Registers
#define HDA_CORBADDR  0x40 // CORB Base Address
#define HDA_CORBWP    0x48 // CORB Write Pointer
#define HDA_CORBRP    0x4A // CORB Read Pointer
#define HDA_CORBCTL   0x4C // CORB Control
#define HDA_CORBSTS   0x4D // CORB Status
#define HDA_CORBSIZE  0x4E // CORB Size

#define HDA_RIRBADDR  0x50 // RIRB Base Address
#define HDA_RIRBWP    0x58 // RIRB Write Pointer
#define HDA_RIRBCTL   0x5C // RIRB Control
#define HDA_RIRBSTS   0x5D // RIRB Status
#define HDA_RIRBSIZE  0x5E // RIRB Size


#pragma endregion

#include <stdint.h>

void hda_init();