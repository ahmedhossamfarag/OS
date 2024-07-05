 #include "low_level.h"

 unsigned char port_byte_in(unsigned short port) {
    // A handy C wrapper function that reads a byte from the specified port
    // "=a" (result) means: put AL register in variable RESULT when finished
    // "d" (port) means: load EDX with port
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
 }

 void port_byte_out(unsigned short port, unsigned char data) {
    // "a" (data) means: load EAX with data
    // "d" (port) means: load EDX with port
    __asm__("out %%al, %%dx" : :"a" (data), "d" (port));
 }

 unsigned short port_word_in(unsigned short port) {
    unsigned short result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
 }

 void port_word_out(unsigned short port, unsigned short data) {
    __asm__("out %%ax, %%dx" : :"a" (data), "d" (port));
 }

 
void outsw(unsigned short port, const void *addr, int count) {
    __asm__ volatile ("rep outsw" : "+S"(addr), "+c"(count) : "d"(port));
}

void insw(unsigned short port, void *addr, int count) {
    __asm__ volatile ("rep insw" : "+D"(addr), "+c"(count) : "d"(port));
}


inline void io_wait(){
    // Port 0x80 is used for 'checkpoints' during POST.
    // The Linux kernel uses it for the same purpose, so we shall too.
    // The IO operation is basically a delay.
    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}
