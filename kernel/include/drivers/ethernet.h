#pragma region Ethernet_intel_82540em

// Register Offsets
#define CTRL     0x0000  // Device Control Register
#define STATUS   0x0008  // Device Status Register
#define RCTL     0x0100  // Receive Control Register
#define TCTL     0x0400  // Transmit Control Register
#define RDBAL    0x2800  // Receive Descriptor Base Address Low
#define RDBAH    0x2804  // Receive Descriptor Base Address High
#define RDLEN    0x2808  // Receive Descriptor Length
#define RDH      0x2810  // Receive Descriptor Head
#define RDT      0x2818  // Receive Descriptor Tail
#define TDBAL    0x3800  // Transmit Descriptor Base Address Low
#define TDBAH    0x3804  // Transmit Descriptor Base Address High
#define TDLEN    0x3808  // Transmit Descriptor Length
#define TDH      0x3810  // Transmit Descriptor Head
#define TDT      0x3818  // Transmit Descriptor Tail
#define ICR      0x00C0  // Interrupt Cause Read
#define IMS      0x00D0  // Interrupt Mask Set/Read
#define IMC      0x00D8  // Interrupt Mask Clear
#define RAL0     0x5400  // Receive Address Low
#define RAH0     0x5404  // Receive Address High

// CTRL Register Bits
#define CTRL_FD           (1 << 0)  // Full Duplex
#define CTRL_LRST         (1 << 3)  // Link Reset
#define CTRL_ASDE         (1 << 5)  // Auto-Speed Detection Enable
#define CTRL_SLU          (1 << 6)  // Set Link Up
#define CTRL_ILOS         (1 << 7)  // Invert Loss-Of-Signal
#define CTRL_SPEED_MASK   (3 << 8)  // Speed Selection
#define CTRL_SPEED_10     (0 << 8)
#define CTRL_SPEED_100    (1 << 8)
#define CTRL_SPEED_1000   (2 << 8)
#define CTRL_RESET        (1 << 26) // Device Reset

// STATUS Register Bits
#define STATUS_FD         (1 << 0)  // Full Duplex
#define STATUS_LU         (1 << 1)  // Link Up
#define STATUS_TXOFF      (1 << 4)  // Transmission Paused
#define STATUS_SPEED_MASK (3 << 6)
#define STATUS_SPEED_10   (0 << 6)
#define STATUS_SPEED_100  (1 << 6)
#define STATUS_SPEED_1000 (2 << 6)

// RCTL Register Bits
#define RCTL_EN           (1 << 1)  // Receiver Enable
#define RCTL_SBP          (1 << 2)  // Store Bad Packets
#define RCTL_UPE          (1 << 3)  // Unicast Promiscuous Enable
#define RCTL_MPE          (1 << 4)  // Multicast Promiscuous Enable
#define RCTL_LPE          (1 << 5)  // Long Packet Enable
#define RCTL_LBM_NO       (0 << 6)  // No Loopback
#define RCTL_LBM_MAC      (3 << 6)  // MAC Loopback
#define RCTL_RDMTS_HALF   (0 << 8)  // Receive Descriptor Minimum Threshold Size
#define RCTL_RDMTS_QUARTER (1 << 8)
#define RCTL_RDMTS_EIGHTH (2 << 8)
#define RCTL_MO_36        (0 << 12) // Multicast Offset - bits 47:36
#define RCTL_MO_35        (1 << 12) // Multicast Offset - bits 46:35
#define RCTL_MO_34        (2 << 12) // Multicast Offset - bits 45:34
#define RCTL_MO_32        (3 << 12) // Multicast Offset - bits 43:32
#define RCTL_BAM          (1 << 15) // Broadcast Accept Mode
#define RCTL_BSIZE_2048   (0 << 16) // Receive Buffer Size 2048
#define RCTL_BSIZE_1024   (1 << 16) // Receive Buffer Size 1024
#define RCTL_BSIZE_512    (2 << 16) // Receive Buffer Size 512
#define RCTL_BSIZE_256    (3 << 16) // Receive Buffer Size 256
#define RCTL_BSIZE_16384  (1 << 16) // Receive Buffer Size 16384
#define RCTL_BSIZE_8192   (2 << 16) // Receive Buffer Size 8192
#define RCTL_BSIZE_4096   (3 << 16) // Receive Buffer Size 4096

// TCTL Register Bits
#define TCTL_EN           (1 << 1)  // Transmit Enable
#define TCTL_PSP          (1 << 3)  // Pad Short Packets
#define TCTL_CT_SHIFT     4         // Collision Threshold
#define TCTL_COLD_SHIFT   12        // Collision Distance
#define TCTL_SWXOFF       (1 << 22) // Software XOFF Transmission
#define TCTL_RTLC         (1 << 24) // Re-transmit on Late Collision

// TX CMD
#define TXD_CMD_EOP  0x01 // End of Packet
#define TXD_CMD_RS   0x08 // Report Status

// IMS/ICR/IMC Register Bits
#define IMS_TXDW          (1 << 0)  // Transmit Descriptor Written Back
#define IMS_TXQE          (1 << 1)  // Transmit Queue Empty
#define IMS_LSC           (1 << 2)  // Link Status Change
#define IMS_RXSEQ         (1 << 3)  // Receive Sequence Error
#define IMS_RXDMT0        (1 << 4)  // Receive Descriptor Minimum Threshold Reached
#define IMS_RXO           (1 << 6)  // Receive Overrun
#define IMS_RXT           (1 << 7)  // Receiver Timer Interrupt

// Other Constants
#define E1000_RCTL_BSIZE_256   0x00030000
#define E1000_RCTL_BSIZE_512   0x00020000
#define E1000_RCTL_BSIZE_1024  0x00010000
#define E1000_RCTL_BSIZE_2048  0x00000000
#define E1000_RCTL_BSIZE_4096  0x00030000
#define E1000_RCTL_BSIZE_8192  0x00020000
#define E1000_RCTL_BSIZE_16384 0x00010000

#pragma endregion

#define N_DESC 1
#define RX_RING_SIZE (N_DESC*16) // n descriptors, each 16 bytes

#include <stdint.h>

// Transmit Descriptor
typedef struct  {
    uint64_t buffer_addr; // Address of the data buffer
    uint16_t length;      // Length of the data buffer
    uint8_t  cso;         // Checksum offset
    uint8_t  cmd;         // Command field
    uint8_t  status;      // Status field
    uint8_t  css;         // Checksum start
    uint16_t special;     // Special field
} e1000_tx_desc_t;

// Receive Descriptor
typedef struct  {
    uint64_t buffer_addr; // Address of the data buffer
    uint16_t length;      // Length of the data buffer
    uint16_t pkt_checksum; // Packet checksum
    uint8_t  status;      // Status field
    uint8_t  errors;      // Errors field
    uint16_t special;     // Special field
} e1000_rx_desc_t;


void ethernet_init();

void ethernet_send_packet(void* packet, uint32_t length);