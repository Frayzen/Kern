#ifndef RSDP_H
#define RSDP_H

#include "k/types.h"

union other_sdt_ptr {
	u64 xptr;
	u32 ptr;
};


struct RSDT_header {
	char signature[8];
	u8 checksum;
	char OEMID[6];
	u8 revision;
	u32 rsdt_address;
	u32 length;
	u64 xsdtAddress;
	u8 extended_checksum;
	u8 reserved[3];
};

struct RSDT {
  struct RSDT_header h;
	u64 other_sdt[];
} __attribute__((packed));

struct XSDT {
  struct RSDT_header h;
	u64 other_sdt[];
} __attribute__((packed));

void *find_SDT(char sig[4]);

#endif /* !RSDP_H */
