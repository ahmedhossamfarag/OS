#include "libc.h"

void mem_copy(char *from, char *to, unsigned int size)
{
   if(to < from){
        __asm__ __volatile__ (
            "cld\n\t"                      // Clear the direction flag to ensure forward copying
            "rep movsd\n\t"                // Copy 32-bit chunks
            "mov %3, %%ecx\n\t"            // Remaining bytes count
            "rep movsb"                    // Copy remaining bytes
            : /* No output operands */
            : "D" (to), "S" (from), "c" (size / 4), "r" (size % 4)
            : "memory"
        );
   }else if(to > from){
        __asm__ __volatile__ (
            "std\n\t"                      // Set the direction flag to ensure backward copying
            "rep movsd\n\t"                // Copy 32-bit chunks
            "mov %3, %%ecx\n\t"            // Remaining bytes count
            "rep movsb"                    // Copy remaining bytes
            : /* No output operands */
            : "D" (to + size - 1), "S" (from + size - 1), "c" (size / 4), "r" (size % 4)
            : "memory"
        );
   }
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