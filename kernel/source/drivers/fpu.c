#include "fpu.h"
#include "pic.h"
#include "apic.h"


void (*fpu_handler_proc)(uint16_t);

void set_fpu_handler_proc(void (*proc)(uint16_t)){
    fpu_handler_proc = proc;
}


void fpu_handler() {
    // Read the FPU status word to determine the cause of the exception
    uint16_t status;
    asm volatile ("fstsw %0" : "=m" (status));

    if(fpu_handler_proc)
        fpu_handler_proc(status);

    pic_sendEOI(FPU_IRQ);
}


void fpu_enable_mask(uint16_t mask) {
    unsigned int cw;
    _FPU_GETCW(cw);
    cw |= mask;
    _FPU_SETCW(cw);
}

void fpu_clear_mask(uint16_t mask) {
    unsigned int cw;
    _FPU_GETCW(cw);
    cw &= ~mask;
    _FPU_SETCW(cw);
}

void enable_fpu_exceptions() {
    irq_clear_mask(FPU_IRQ);
}


void apic_fpu_handler() {
    // Read the FPU status word to determine the cause of the exception
    uint16_t status;
    asm volatile ("fstsw %0" : "=m" (status));

    if(fpu_handler_proc)
        fpu_handler_proc(status);

    apic_sendEOI();
}

