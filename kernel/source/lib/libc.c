#include "libc.h"

void mem_copy(char *from, char *to, unsigned int size)
{
    __asm__ __volatile__ (
        "cld\n\t"                      // Clear the direction flag to ensure forward copying
        "rep movsd\n\t"                // Copy 32-bit chunks
        "mov %3, %%ecx\n\t"            // Remaining bytes count
        "rep movsb"                    // Copy remaining bytes
        : /* No output operands */
        : "D" (to), "S" (from), "c" (size / 4), "r" (size % 4)
        : "memory"
    );
}

char *mem_set(char *dest, char val, unsigned int count) {
    asm volatile (
        "cld\n\t"            // Clear the direction flag to ensure forward direction
        "rep stosb"          // Repeat the `stosb`(store string byte) instruction `count` times
        :                    // No output operands
        : "a" (val), "D" (dest), "c" (count)  // Input operands: AL (value), EDI (destination), and ECX (count)
        : "memory"           // Clobber list: memory
    );
    return dest;
}