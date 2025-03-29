#ifndef SDT_H
#define SDT_H

#include "k/types.h"

struct SDT_header {
	char signature[4];
	u32 length;
	u8 revision;
	u8 checksum;
	char OEMID[6];
	char OEM_table_id[8];
	u32 OEM_revision;
	u32 creator_id;
	u32 creator_revision;
};

#define GET_SDT(Signature, Type) (Type *)find_SDT(#Signature)

struct MADT {
	struct SDT_header h;
	u32 apic_addr;
	u32 flag;
};
#define FIND_MADT (GET_SDT(MADT, struct MADT))

#endif /* !SDT_H */
