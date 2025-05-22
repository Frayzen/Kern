#ifndef IO_APIC_H
#define IO_APIC_H

#include "drivers/apic/madt.h"
#include "k/compiler.h"
#include "k/types.h"

void io_apic_setup(struct madt *madt);
u32 read_io_apic(u32 reg);
void write_io_apic(u32 reg, u32 value);

#define IO_APIC_VERSION 0x01 // RO
// 0x10 to 0x3F	Contains a list of redirection entries. They can be read from and written to. Each entries uses two addresses, e.g. 0x12 and 0x13
#define IO_APIC_REDIR 0x010

struct io_apic_redir {
  u8 int_vector; // allowed  values from 0x10 to 0xfe 
  u8 delivery_type : 2;
  u8 will_be_sent : 1; // ro
  u8 polarity : 1;
  u8 trigger_state : 1; //ro
  u8 int_mask : 1;
  u64 __unused : 39;
  u8 dest_field;
} __packed;

#endif /* !IO_APIC_H */
