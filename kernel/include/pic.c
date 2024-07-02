#include "pic.h"
#include "low_level.h"

void pic_sendEOI(uint8_t irq)
{
	if(irq >= 8)
		port_byte_out(PIC_S_COMMAND,PIC_EOI);
	
	port_byte_out(PIC_M_COMMAND,PIC_EOI);
}

void pic_remap()
{
	uint8_t a1, a2;
	
	a1 = port_byte_in(PIC_M_DATA);                        // save masks
	a2 = port_byte_in(PIC_S_DATA);
	
	port_byte_out(PIC_M_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    io_wait();
	
	port_byte_out(PIC_S_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
	
	port_byte_out(PIC_M_DATA, PIC_M_OFFSET);                 // ICW2: Master PIC vector offset
    io_wait();
	
	port_byte_out(PIC_S_DATA, PIC_S_OFFSET);                 // ICW2: Slave PIC vector offset
    io_wait();
	
	port_byte_out(PIC_M_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    io_wait();
	
	port_byte_out(PIC_S_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();
	
	
	port_byte_out(PIC_M_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    io_wait();
	
	port_byte_out(PIC_S_DATA, ICW4_8086);
    io_wait();
	
	
	port_byte_out(PIC_M_DATA, a1);   // restore saved masks.
	port_byte_out(PIC_S_DATA, a2);
}

void pic_disable(void) {
    port_byte_out(PIC_M_DATA, PIC_DISABLE);
    port_byte_out(PIC_S_DATA, PIC_DISABLE);
}

void irq_set_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;

    if(irq_line < 8) {
        port = PIC_M_DATA;
    } else {
        port = PIC_S_DATA;
        irq_line -= 8;
    }
    value = port_byte_in(port) | (1 << irq_line);
    port_byte_out(port, value);        
}

void irq_clear_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;

    if(irq_line < 8) {
        port = PIC_M_DATA;
    } else {
        port = PIC_S_DATA;
        irq_line -= 8;
    }
    value = port_byte_in(port) & ~(1 << irq_line);
    port_byte_out(port, value);        
}

uint16_t __pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    port_byte_out(PIC_M_COMMAND, ocw3);
    port_byte_out(PIC_S_COMMAND, ocw3);
    return (port_byte_in(PIC_S_COMMAND) << 8) | port_byte_in(PIC_M_COMMAND);
}

uint16_t pic_get_irr(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}

uint16_t pic_get_isr(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}
