#include <stdint.h>
#include "iolib.h"

uint8_t process_create(uint32_t pid, FILE* file);

uint8_t thread_create(uint32_t tid, void* start, uint32_t stack);

extern "C" void process_exit();

void thread_exit();

uint8_t process_terminate(uint32_t pid);

uint8_t thread_terminate(uint32_t tid);

