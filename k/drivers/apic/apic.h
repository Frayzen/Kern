#ifndef APIC_H
#define APIC_H

#include "k/compiler.h"
#include "k/types.h"
struct apic_base { // 64 bits
	u8 __reserved0 : 8;
	u8 bsp : 1; // bootsrap processor
	u8 __reserved1 : 2;
	u8 enable : 1;
	u32 base : 24;
	u32 __reserved2 : 28;
};

struct apic_reg {
	u8 vector_nb;
	u8 is_nmi : 3; // reserved for timer
	u8 __reserved0 : 1;
	u8 int_pending : 1;
	u8 is_low_trig : 1; // reserved for timer
	u8 remote_irr : 1; // reserved for timer
	u8 is_lvl_trig : 1; // reserved for timer
	u8 masked : 1;
	u16 __reserved1 : 15;
} __packed;

enum apic_timer_mode {
	ONE_SHOT = 0,
	PERIODIC = 1,
	TSC = 2,
};

#define LAPIC_LVT_TIMER_ONESHOT (0 << 17)
#define LAPIC_LVT_TIMER_PERIODIC (1 << 17)
#define LAPIC_LVT_TIMER_TSCDEADLINE (2 << 17)
#define LAPIC_LVT_MASKED (1 << 16)

void apic_setup(void);
void apic_send_eoi(void);

extern u32 volatile *lapic;

#endif /* !APIC_H */
