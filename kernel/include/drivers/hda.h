#pragma region HDA

// Global Registers
#define HDA_GCAP      0x0   // Global Capabilites // 2 bytes
#define HDA_GCTL      0x08  // Global Control // 4 bytes
#define HDA_OUTPAY    0x04  // Ouptput Payload // 2 bytes
#define HDA_INPAY     0x08  // Input Payload // 2 bytes
#define HDA_WAKEEN    0x0C  // Wake Enable // 2 bytes
#define HDA_STATESTS   0x0E  // Wake Status // 2 bytes
#define HDA_GSTS      0x10  // Global Status // 2 bytes
#define HDA_OUTSTRMPAY 0x18  // Ouptput Stream Payload // 2 bytes
#define HDA_INSTRMPAY  0x1A  // Input Stream Payload // 2 bytes

#define HDA_GCTL_CRST 0x00000001

// Interrupt Registers
#define HDA_INTCTL    0x20  // Interrupt Control // 4 bytes
#define HDA_INTSTS    0x24  // Interrupt Status // 4 bytes

#define HDA_WALCLK   0x30  // Wall Clock Counter // 4 bytes
#define HDA_SSYNC    0x38  // Stream Synckronization // 4 bytes

// Codec Communication Registers
#define HDA_CORBADDR  0x40 // CORB Base Address // 4 bytes
#define HDA_CORBWP    0x48 // CORB Write Pointer // 2 bytes
#define HDA_CORBRP    0x4A // CORB Read Pointer // 2 bytes
#define HDA_CORBCTL   0x4C // CORB Control // 1 byte
#define HDA_CORBSTS   0x4D // CORB Status // 1 byte
#define HDA_CORBSIZE  0x4E // CORB Size // 1 byte

#define HDA_RIRBADDR  0x50 // RIRB Base Address // 4 bytes
#define HDA_RIRBWP    0x58 // RIRB Write Pointer // 2 bytes
#define HDA_RINTCNT   0x5A // Response Interrupt Count // 2 bytes
#define HDA_RIRBCTL   0x5C // RIRB Control // 1 byte
#define HDA_RIRBSTS   0x5D // RIRB Status // 1 byte
#define HDA_RIRBSIZE  0x5E // RIRB Size // 1 byte

#define HDA_DPIBLBASE 0x70 // DMA Pos Buffer Lower Base // 4 bytes

#define HDA_ISDnCTL(n)    (0x80 + n*0x20) // Input Stream Descriptor n // 2 bytes
#define HDA_ISDnSTS(n)    (0x83 + n*0x20) // ISDn Status // 1 byte
#define HDA_ISDnLPIB(n)   (0x84 + n*0x20) // ISDn Link Pos in Curr Buffer // 4 bytes
#define HDA_ISDnCBL(n)    (0x88 + n*0x20) // ISDn Cyclic Buffer Length // 4 bytes
#define HDA_ISDnLVI(n)    (0x8C + n*0x20) // ISDn Last Valid Indx // 2 bytes
#define HDA_ISDnFIFOS(n)  (0x90 + n*0x20) // ISDn FIFO Size // 2 bytes
#define HDA_ISDnFMT(n)    (0x92 + n*0x20) // ISDn Format // 2 bytes
#define HDA_ISDnBDPL(n)   (0x98 + n*0x20) // ISDn Buffer Descriptor List Pointer Lower // 4 bytes


#define HDA_OSDnCTL(n)    (x+0x00 + n*0x20) // Output Stream Descriptor n // 2 bytes
#define HDA_OSDnSTS(n)    (x+0x03 + n*0x20) // OSDn Status // 1 byte
#define HDA_OSDnLPIB(n)   (x+0x04 + n*0x20) // OSDn Link Pos in Curr Buffer // 4 bytes
#define HDA_OSDnCBL(n)    (x+0x08 + n*0x20) // OSDn Cyclic Buffer Length // 4 bytes
#define HDA_OSDnLVI(n)    (x+0x0C + n*0x20) // OSDn Last Valid Indx // 2 bytes
#define HDA_OSDnFIFOS(n)  (x+0x10 + n*0x20) // OSDn FIFO Size // 2 bytes
#define HDA_OSDnFMT(n)    (x+0x12 + n*0x20) // OSDn Format // 2 bytes
#define HDA_OSDnBDPL(n)   (x+0x18 + n*0x20) // OSDn Buffer Descriptor List Pointer Lower // 4 bytes


#define HDA_BSDnCTL(n)    (y+0x00 + n*0x20) // Bidirection Stream Descriptor n // 2 bytes
#define HDA_BSDnSTS(n)    (y+0x03 + n*0x20) // BSDn Status // 1 byte
#define HDA_BSDnLPIB(n)   (y+0x04 + n*0x20) // BSDn Link Pos in Curr Buffer // 4 bytes
#define HDA_BSDnCBL(n)    (y+0x08 + n*0x20) // BSDn Cyclic Buffer Length // 4 bytes
#define HDA_BSDnLVI(n)    (y+0x0C + n*0x20) // BSDn Last Valid Indx // 2 bytes
#define HDA_BSDnBDPL(n)   (y+0x18 + n*0x20) // BSDn Buffer Descriptor List Pointer Lower // 4 bytes

#define HDA_ICW     0x60 // Immediate Command Write // 4 bytes
#define HDA_ICR     0x64 // Immediate Command Read // 4 bytes
#define HDA_ICS     0x68 // Immediate Command Status // 2 byte

#pragma endregion

#include <stdint.h>

typedef struct
{
    uint64_t address;
    uint32_t length;
    uint32_t ioc: 1;
    uint32_t reserved: 31;
}__attribute__((packed)) hda_bdl_entry_t;

typedef struct
{
    uint32_t verb: 20;
    uint32_t node_id: 8;
    uint32_t codec_address: 4;
}__attribute__((packed)) hda_corb_entry_t;

typedef struct
{
    uint32_t response; 
    uint32_t line: 4;
    uint32_t solicited: 1;
    uint32_t reserved: 27;
}__attribute__((packed)) hda_rirb_entry_t;



void hda_init();