#include "fpu.h"
#include "pic.h"


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

    // Clear the exception flags in the status word
    asm volatile ("fnclex");

    pic_sendEOI(FPU_IRQ);
}


void configure_fpu(uint16_t mask) {
    unsigned int cw;
    _FPU_GETCW(cw);
    cw |= mask;
    _FPU_SETCW(cw);
}

