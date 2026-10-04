#include <stdint.h>
#include "iolib.h"

uint8_t process_create(uint64_t pid, FILE* file);

uint8_t thread_create(uint64_t tid, void* start, uint64_t stack);

extern "C" void process_exit();

void thread_exit();

uint8_t process_terminate(uint64_t pid);

uint8_t thread_terminate(uint64_t tid);

