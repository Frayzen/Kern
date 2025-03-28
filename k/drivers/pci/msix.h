#ifndef MSIX_H
#define MSIX_H

#include "drivers/pci/pci.h"
#include "k/compiler.h"
#include "k/types.h"

#define MSIX_MXID 0x0
struct MSIX_identifier {
	u8 capID;
	u8 nextPtr;
} __packed;

#define MSIX_MXC 0x2
struct MSIX_msg_ctl {
	u16 table_size : 11;
	u8 __reserved0 : 3;
	u8 function_mask : 1;
	u8 enable : 1;
};

#define MSIX_MTAB 0x4

struct MSIX_vector_table {
	u32 __reserved0 : 31;
	u8 masked : 1;
	u32 msg_data;
	u64 msg_addr; // mask the low bits (& 0xFFFFFFF0)
} __packed;

void enable_msix(struct pci_device *dev);

#endif /* !MSIX_H */
