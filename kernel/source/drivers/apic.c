#include "apic.h"
#include "screen_print.h"
#include "strlib.h"
#include "interrupt.h"
#include "pic.h"

uint8_t apic_detected;

volatile uint32_t *lapic;
volatile uint32_t *ioapic;

extern void disable_pic();

extern uint32_t detect_apic();

extern void enable_apic();


void initialize_lapic(){
    set_lapic(LAPIC_SPURIOUS,0x1FF);  // Enable APIC and set spurious interrupt vector to 0xFF
    set_lapic(LAPIC_TPR, 0x10000); // Set Task Priority Register to 0 to accept all interrupts
}


void apic_init()
{
    lapic = (volatile uint32_t *)APIC_BASE;
    ioapic = (volatile uint32_t *)IOAPIC_BASE;
    apic_detected = 0;

    if(detect_apic()){
        apic_detected = 1;
        disable_pic();
        enable_apic();
        initialize_lapic();
        init_ioapic();
    }
}

void set_lapic(uint32_t offset, uint32_t value)
{
    lapic[offset / 4] = value;
}

uint32_t get_lapic(uint32_t offset)
{
    return lapic[offset/4];
}

void apic_sendEOI()
{
    set_lapic(LAPIC_EOI, 0);
}

#pragma region Timer

void (*lapic_timer_handler_proc) (cpu_state_t*);

void set_lapic_timer_handler_proc(void (*proc) (cpu_state_t*)){
    lapic_timer_handler_proc = proc;
}

void lapic_timer_handler(cpu_state_t* state){
    if(lapic_timer_handler_proc)
        lapic_timer_handler_proc(state);
    apic_sendEOI();
}

extern void isr_lapic_timer_handler();

void enable_lapic_timer(uint32_t initial, uint32_t mode){
    if(!apic_detected) return;
    set_idt_entry(TIMER_INT, (uint32_t)isr_lapic_timer_handler);
    set_lapic(LAPIC_LVT_TIMER, TIMER_INT | mode);
    set_lapic(LAPIC_TIMER_DIV, LAPIC_TIMER_DIV_16);
    set_lapic(LAPIC_TIMER_INIT, initial);
}

#pragma endregion

#pragma region IOAPIC


void set_ioapic(uint32_t offset, uint32_t value){
    ioapic[offset / 4] = value;
}

uint32_t get_ioapic(uint32_t offset){
    return ioapic[offset/4];
}

void ioapic_write(uint32_t reg, uint32_t value) {
    set_ioapic(IOAPIC_REGSEL, reg);
    set_ioapic(IOAPIC_WINDOW, value);
}

uint32_t ioapic_read(uint32_t reg) {
    set_ioapic(IOAPIC_REGSEL, reg);
    return get_ioapic(IOAPIC_WINDOW);
}

void ioapic_set_irq(uint8_t irq, uint32_t vector, uint32_t apic_id){
    ioapic_write(0x10 + 2 * irq, vector);
    ioapic_write(0x10 + 2 * irq + 1, (apic_id << 24));
}

extern void isr_apic_timer_handler();
extern void isr_apic_keyboard_handler();
extern void isr_apic_mouse_handler();
extern void isr_apic_rtc_handler();
extern void isr_apic_fpu_handler();

void init_ioapic(){
    ioapic_set_irq(0, PIC_M_OFFSET, 0);
    ioapic_set_irq(1, PIC_M_OFFSET + 1, 0);
    ioapic_set_irq(8, PIC_M_OFFSET + 8, 0);
    ioapic_set_irq(12, PIC_M_OFFSET + 12, 0);
    ioapic_set_irq(13, PIC_M_OFFSET + 13, 0);


    set_idt_entry(PIC_M_OFFSET, (uint32_t)(isr_apic_timer_handler));
    set_idt_entry(PIC_M_OFFSET + 1, (uint32_t)(isr_apic_keyboard_handler));
    set_idt_entry(PIC_M_OFFSET + 8, (uint32_t)(isr_apic_rtc_handler));
    set_idt_entry(PIC_M_OFFSET + 12, (uint32_t)(isr_apic_mouse_handler));
    set_idt_entry(PIC_M_OFFSET + 13, (uint32_t)(isr_apic_fpu_handler));
}


#pragma endregion