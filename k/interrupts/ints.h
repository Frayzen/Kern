#ifndef INTS_H
#define INTS_H

#include "isr_list.h"
#include "k/kfs.h"

void setup_idt(void);
void pic_send_eoi(unsigned int irq);

enum gate_type : u8 {
  TASK_GATE = 0x5,
  INT_GATE_16B = 0x6,
  TRAP_GATE_16B = 0x7,
  INT_GATE_32B = 0xE,
  TRAP_GATE_32B = 0xF,
};

struct gate_descriptor {
	u16 offset_low : 16;
	u16 segment_selector : 16;
	u8 __unused : 8;
	enum gate_type type : 5;
	u8 privilege : 2; // (ring of privilege)
	u8 present : 1;
	u16 offset_high : 16;
} __packed;

struct idt_descriptor {
	u16 limit;
	u32 base;
} __packed;


#define X(id, key, name, errcode) extern void isr##key(void);
ISR_LIST
IRQ_LIST
#undef X

#define FN_PTR(Fn) (uint_ptr)((void (*)(void))(Fn))
#define OFFSET_LOW(Fn) (0xFFFF & (FN_PTR(Fn)))
#define OFFSET_HIGH(Fn) ((0xFFFF0000 & (FN_PTR(Fn))) >> 16)


#endif /* !INTS_H */
