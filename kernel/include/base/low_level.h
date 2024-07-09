 #define inb port_byte_in
 #define outb port_byte_out
 
 unsigned char port_byte_in(unsigned short port);

 void port_byte_out(unsigned short port, unsigned char data);

 unsigned short port_word_in(unsigned short port);

 void port_word_out(unsigned short port, unsigned short data);
 
void outsw(unsigned short port, const void *addr, int count);

void insw(unsigned short port, void *addr, int count);

 void io_wait(void);
