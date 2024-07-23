#include <stdint.h>

uint32_t fopen_handler();

uint32_t fsize_handler();

uint32_t fclose_handler();

uint32_t fread_handler();

uint32_t fwrite_handler();

uint32_t prints_handler();

uint32_t scans_handler();

uint32_t scans_line_handler();

void io_syscall_init();