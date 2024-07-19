#define APIC_BASE 0xFEE00000
#define IOAPIC_BASE 0xFEC00000

#define LAPIC_ID              0x020  // Local APIC ID
#define LAPIC_VERSION         0x030  // Local APIC Version
#define LAPIC_TPR             0x080  // Task Priority Register
#define LAPIC_APR             0x090  // Arbitration Priority Register
#define LAPIC_PPR             0x0A0  // Processor Priority Register
#define LAPIC_EOI             0x0B0  // End of Interrupt
#define LAPIC_RRD             0x0C0  // Remote Read
#define LAPIC_LOGICAL_DEST    0x0D0  // Logical Destination
#define LAPIC_DEST_FORMAT     0x0E0  // Destination Format
#define LAPIC_SPURIOUS        0x0F0  // Spurious Interrupt Vector
#define LAPIC_ISR_BASE        0x100  // In-Service Register Base (8 registers)
#define LAPIC_TMR_BASE        0x180  // Trigger Mode Register Base (8 registers)
#define LAPIC_IRR_BASE        0x200  // Interrupt Request Register Base (8 registers)
#define LAPIC_ESR             0x280  // Error Status Register
#define LAPIC_ICR_LO          0x300  // Interrupt Command Register (Low)
#define LAPIC_ICR_HI          0x310  // Interrupt Command Register (High)
#define LAPIC_LVT_TIMER       0x320  // Local Vector Table (LVT) Timer
#define LAPIC_LVT_THERMAL     0x330  // LVT Thermal Sensor
#define LAPIC_LVT_PERF        0x340  // LVT Performance Monitoring Counters
#define LAPIC_LVT_LINT0       0x350  // LVT LINT0
#define LAPIC_LVT_LINT1       0x360  // LVT LINT1
#define LAPIC_LVT_ERROR       0x370  // LVT Error
#define LAPIC_TIMER_INIT      0x380  // Timer Initial Count
#define LAPIC_TIMER_CURR      0x390  // Timer Current Count
#define LAPIC_TIMER_DIV       0x3E0  // Timer Divide Configuration

// LAPIC Timer Divide Configuration Values
#define LAPIC_TIMER_DIV_2     0x0
#define LAPIC_TIMER_DIV_4     0x1
#define LAPIC_TIMER_DIV_8     0x2
#define LAPIC_TIMER_DIV_16    0x3
#define LAPIC_TIMER_DIV_32    0x8
#define LAPIC_TIMER_DIV_64    0x9
#define LAPIC_TIMER_DIV_128   0xA
#define LAPIC_TIMER_DIV_1     0xB

// LAPIC Timer Modes
#define LAPIC_TIMER_PERIODIC_MODE 0x20000  // Bit 16 set to 1 for periodic mode
#define LAPIC_TIMER_ONE_SHOT_MODE 0x00000  // Bit 16 set to 0 for one-shot mode


#define TIMER_INT 0x20

#include <stdint.h>
#include "process.h"

void apic_init();

void set_lapic(uint32_t offset, uint32_t value);

uint32_t get_lapic(uint32_t offset);

void apic_sendEOI();

// Timer

void set_lapic_timer_handler_proc(void (*proc) (cpu_state_t*));

void lapic_timer_handler(cpu_state_t* state);

void enable_lapic_timer(uint32_t initial, uint32_t mode);

// IOAPIC

#define IOAPIC_REGSEL 0x00
#define IOAPIC_WINDOW 0x10

void set_ioapic(uint32_t offset, uint32_t value);

uint32_t get_ioapic(uint32_t offset);

void init_ioapic();