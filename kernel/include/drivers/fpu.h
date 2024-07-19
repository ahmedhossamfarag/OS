// Floating Point Unit

/* Macros for accessing the hardware control word.  */

#define _FPU_DEFAULT 0x037f
#define _FPU_IEEE 0x037f

/* Macros for accessing the hardware control word.  */

#define _FPU_EXTENDED 0x300
#define _FPU_DOUBLE 0x200
#define _FPU_SINGLE 0x0

/* Bit masks for exception masks in the FPU control word.  */

#define _FPU_MASK_IM 0x01   /* Invalid operation mask.  */
#define _FPU_MASK_DM 0x02   /* Denormalized operand mask.  */
#define _FPU_MASK_ZM 0x04   /* Divide-by-zero mask.  */
#define _FPU_MASK_OM 0x08   /* Overflow mask.  */
#define _FPU_MASK_UM 0x10   /* Underflow mask.  */
#define _FPU_MASK_PM 0x20   /* Precision mask.  */
#define _FPU_MASK_ALL 0x7F   /* All masks.  */

/* Functions for accessing the FPU control word.  */

#define _FPU_GETCW(cw) __asm__ __volatile__ ("fstcw %0" : "=m" (cw))
#define _FPU_SETCW(cw) __asm__ __volatile__ ("fldcw %0" : : "m" (cw))


#define FPU_INVLID_OP 0x1
#define FPU_DIV_BY_ZERO 0x4
#define FPU_OVERFLOW 0x8
#define FPU_UNDERFLOW 0x10
#define FPU_PRECISION_ERR 0x20

#define FPU_IRQ 13

#include <stdint.h> 


void set_fpu_handler_proc(void (*proc)(uint16_t));

/* IRQ 13 */
void fpu_handler();

/* enable fpu exceptions */
void fpu_enable_mask(uint16_t mask);

/* disable fpu exceptions */
void fpu_clear_mask(uint16_t mask);

void enable_fpu_exceptions();

void apic_fpu_handler();