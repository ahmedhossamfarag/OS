#include "memlib.h"

void* operator new(uint32_t size) {
    void* p = malloc(size);
    return p;
}

// Overloading the global delete operator
void operator delete(void* p, uint32_t sz) {
    mfree(p, sz);
}

void* operator new[](uint32_t size) {
    void* p = malloc(size);
    return p;
}

// Overloading the global delete operator
void operator delete[](void* p, uint32_t sz) {
    mfree(p, sz);
}