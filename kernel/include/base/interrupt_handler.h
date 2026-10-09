#include "process.h"

void interrupt_handler_init();

void exception_handler(cpu_state_t*);

void divide_error_handler(cpu_state_t*);

void debug_exception_handler(cpu_state_t*);

void nmi_handler(cpu_state_t*); // Non-maskable interrupt

void breakpoint_handler(cpu_state_t*);

void overflow_handler(cpu_state_t*);

void bound_range_exceeded_handler(cpu_state_t*);

void invalid_opcode_handler(cpu_state_t*);

void device_not_available_handler(cpu_state_t*);

void double_fault_handler(cpu_error_state_t*);

void coprocessor_segment_overrun_handler(cpu_state_t*);

void invalid_TSS_handler(cpu_error_state_t*);

void segment_not_present_handler(cpu_error_state_t*);

void stack_segment_fault_handler(cpu_error_state_t*);

void general_protection_fault_handler(cpu_error_state_t*);

void page_fault_handler(cpu_error_state_t*);

void x87_floating_point_handler(cpu_state_t*);

void alignment_check_handler(cpu_error_state_t*);

void machine_check_handler(cpu_state_t*);

void simd_exception_handler(cpu_state_t*);

void virtualization_exception_handler(cpu_state_t*);



void pic_handler(cpu_state_t*);



void syscall_handler(cpu_state_t*);
