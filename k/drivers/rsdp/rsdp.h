#ifndef RSDP_H
#define RSDP_H

// Root System Description Table (XSDT for 64 bits system)

#include "drivers/rsdp/sdt.h"
#include "k/compiler.h"
#include "k/types.h"

union other_sdt_ptr {
	u64 xptr;
	u32 ptr;
};


struct RSDP {
	char signature[8];
	u8 checksum;
	char OEMID[6];
	u8 revision;
	u32 rsdt_address;
	u32 length;
	u64 xsdt_address;
	u8 extended_checksum;
	u8 reserved[3];
} __packed;

struct RSDT {
  struct sdt_header h;
	u32 other_sdt[];
} __attribute__((packed));

struct XSDT {
  struct sdt_header h;
	u64 other_sdt[];
} __attribute__((packed));

void *find_SDT(char sig[4]);

#endif /* !RSDP_H */
