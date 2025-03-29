#ifndef APIC_H
#define APIC_H


#include "k/types.h"
struct IA32_APIC_base { // 64 bits
  u8 __reserved0 : 8;
  u8 bsp : 1; // bootsrap processor
  u8 __reserved1 : 2;
  u8 enable : 1;
  u32 base : 24;
  u32 __reserved2 : 28;
};

void acpi_setup(void);


#endif /* !APIC_H */
