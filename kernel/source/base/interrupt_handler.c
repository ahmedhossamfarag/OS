#include "interrupt_handler.h"
#include "vga_print.h"
#include "pic.h"
#include "interrupt.h"
#include "apic.h"
#include "strlib.h"
#include "syscall_map.h"
#include "scheduler.h"
#include "info.h"

extern void isr_exception_handler();
extern void divide_error_handler();
extern void debug_exception_handler();
extern void nmi_handler();
extern void breakpoint_handler();
extern void overflow_handler();
extern void bound_range_exceeded_handler();
extern void invalid_opcode_handler();
extern void device_not_available_handler();
extern void double_fault_handler();
extern void coprocessor_segment_overrun_handler();
extern void invalid_TSS_handler();
extern void segment_not_present_handler();
extern void stack_segment_fault_handler();
extern void general_protection_fault_handler();
extern void page_fault_handler();
extern void x87_floating_point_handler();
extern void alignment_check_handler();
extern void machine_check_handler();
extern void simd_exception_handler();
extern void virtualization_exception_handler();
extern void isr_syscall_handler();

void interrupt_handler_init()
{
    idt_set_entry(DIVIDE_ERROR_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(DEBUG_EXCEPTION_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(NMI_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(BREAKPOINT_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(OVERFLOW_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(BOUND_RANGE_EXCEEDED_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(INVALID_OPCODE_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(DEVICE_NOT_AVAILABLE_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(DOUBLE_FAULT_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(COPROCESSOR_SEGMENT_OVERRUN_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(INVALID_TSS_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(SEGMENT_NOT_PRESENT_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(STACK_SEGMENT_FAULT_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(GENERAL_PROTECTION_FAULT_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(PAGE_FAULT_INT, (uint64_t)isr_exception_handler);    
    idt_set_entry(X87_FLOATING_POINT_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(ALIGNMENT_CHECK_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(MACHINE_CHECK_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(SIMD_EXCEPTION_INT, (uint64_t)isr_exception_handler);
    idt_set_entry(VIRTUALIZATION_EXCEPTION_INT, (uint64_t)isr_exception_handler);
    
    idt_set_user_entry(SYSCALL_INT, (uint64_t)isr_syscall_handler);
}

void exception_handler(cpu_state_t* cpu)
{
    print("\nException Handler\n");
    char s[20];
    print("PID: ")
    println(int_to_hex_str(get_current_process()->pid, s))
    print("TID: ")
    println(int_to_hex_str(get_current_thread()->tid, s))
    print("CR3: ")
    println(int_to_hex_str(get_current_process()->cr3, s))
    print("CS: ")
    println(int_to_hex_str(cpu->cs, s))
    print("RIP: ")
    println(int_to_hex_str(cpu->rip, s))
    print("RSP: ")
    println(int_to_hex_str(cpu->user_rsp, s))
    if(get_current_process() != get_default_process()){
        schedule_thread_terminated(cpu);
    }   
    pic_sendEOI_helper();
}

void divide_error_handler(cpu_state_t*){

}

void debug_exception_handler(cpu_state_t*){

}

void nmi_handler(cpu_state_t*){

}

void breakpoint_handler(cpu_state_t*){

}

void overflow_handler(cpu_state_t*){

}

void bound_range_exceeded_handler(cpu_state_t*){

}

void invalid_opcode_handler(cpu_state_t*){

}

void device_not_available_handler(cpu_state_t*){

}

void double_fault_handler(cpu_error_state_t*){

}

void coprocessor_segment_overrun_handler(cpu_state_t*){

}

void invalid_TSS_handler(cpu_error_state_t*){

}

void segment_not_present_handler(cpu_error_state_t*){

}

void stack_segment_fault_handler(cpu_error_state_t*){

}

void general_protection_fault_handler(cpu_error_state_t*){

}

void page_fault_handler(cpu_error_state_t* state){
    uint64_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

    uint64_t error_code = state->error_code;

    asm volatile("hlt"); // Halt the CPU for debugging purposes

    // Analyze the faulting address and error code
    if (!(error_code & 0x1)) {
        // Page not present
    } else {
        // Page protection violation
        if (error_code & 0x2) {
            // Write operation
        } else {
            // Read operation
        }
        if (error_code & 0x4) {
            // Fault occurred in user mode
        } else {
            // Fault occurred in kernel mode
        }
    }
}

void x87_floating_point_handler(cpu_state_t*){

}

void alignment_check_handler(cpu_error_state_t*){

}

void machine_check_handler(cpu_state_t*){

}

void simd_exception_handler(cpu_state_t*){

}

void virtualization_exception_handler(cpu_state_t*){

}



void pic_handler(cpu_state_t*){
    print("\nPIC Handler\n");
    pic_sendEOI_helper();
}



extern void (*syscall_map[NUM_SYSCALL])(cpu_state_t*);

void syscall_handler(cpu_state_t* state){
    uint64_t n;
    asm("mov %%rsi, %0":"=m"(n));

    if(n >= NUM_SYSCALL || !syscall_map[n]){
        state->rax = 0;
        return;
    }
    
    // uint64_t handler = (uint64_t)syscall_map[n];
    // handler = MEMORY_PHYS_TO_VIRT(handler);
    // ((void (*)(cpu_state_t*))handler)(state);
    syscall_map[n](state);
}
