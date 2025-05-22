#ifndef MADT_H
#define MADT_H

#include "drivers/rsdp/sdt.h"
#include "k/types.h"

enum madt_entry_type : u8 {
  PROCESSOR_LOCAL_APIC = 0, 
  IO_APIC = 1,
  IO_APIC_INT_SRC_OVERRIDE = 2,
  IO_APIC_NON_MASKBL_INT_SRC = 3,
  LOCAL_APIC_NON_MASKBL_INTS = 4,
  LOCAL_APIC_ADDR_OVERRIDE = 5,
  PROCESSOR_LOCAL_X2APIC = 9,
};

struct madt_entry {
	enum madt_entry_type type;
	u8 length;
	u8 data[];
};
struct MADT {
	struct SDT_header h;
	u32 lapic_addr;
	u32 flag;
	struct madt_entry first_entry;
};

struct MADT_io_apic_data {
  u8 id; 
  u8 __reserved; 
  u32 address; 
  u32 global_sys_int_base; 
};

// returns null if not found
struct madt_entry* madt_find_next_entry(struct MADT* madt, struct madt_entry* cur_entry, enum madt_entry_type type);

// equivalent to adt_find_next_entry(madt, &madt.first_entry, type)
struct madt_entry* madt_find_entry(struct MADT* madt, enum madt_entry_type type);

#define FIND_MADT (GET_SDT(APIC, struct MADT))

#endif /* !MADT_H */
