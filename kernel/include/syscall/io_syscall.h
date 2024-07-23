#include "process.h"
#include <stdint.h>

uint32_t fopen_handler(cpu_state_t*);

uint32_t fsize_handler(cpu_state_t*);

uint32_t fclose_handler(cpu_state_t*);

uint32_t fread_handler(cpu_state_t*);

uint32_t fwrite_handler(cpu_state_t*);

uint32_t prints_handler(cpu_state_t*);

uint32_t scans_handler(cpu_state_t*);

uint32_t scans_line_handler(cpu_state_t*);

void io_syscall_init();