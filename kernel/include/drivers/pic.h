#define PIC_M		0x20		/* IO base address for master PIC */
#define PIC_S		0xA0		/* IO base address for slave PIC */
#define PIC_M_COMMAND	PIC_M
#define PIC_M_DATA	(PIC_M+1)
#define PIC_S_COMMAND	PIC_S
#define PIC_S_DATA	(PIC_S+1)

#define PIC_EOI		0x20		/* End-of-interrupt command code */

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */


#define PIC_M_OFFSET 0x20
#define PIC_S_OFFSET 0x28

#define PIC_DISABLE 0xff

#define PIC_READ_IRR                0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR                0x0b    /* OCW3 irq service next CMD read */


#include <stdint.h>

void pic_init();

void pic_sendEOI(uint8_t irq);

void pic_remap();

void pic_disable(void);

/* Disable getting IRQ x*/
void irq_set_mask(uint8_t irq_line);

/* Enable getting IRQ x*/
void irq_clear_mask(uint8_t irq_line);

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void);

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void);