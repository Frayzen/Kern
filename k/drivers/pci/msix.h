#ifndef MSIX_H
#define MSIX_H

#include "drivers/pci/pci.h"
#include "k/compiler.h"
#include "k/types.h"

#define MSIX_MXID 0x0
struct MSIX_identifier {
	u8 capID;
	u8 nextPtr;
	u16 table_size : 11;
	u8 __reserved0 : 3;
	u8 function_mask : 1;
	u8 enable : 1;
} __packed;

#define MSIX_MTAB 0x4

struct MSIX_vector_table {
	u64 msg_addr; // mask the low bits (& 0xFFFFFFF0)
	u32 msg_data;
	u32 masked; // either 0 or 1
} __packed;

void enable_msix(struct pci_device *dev);
void disable_msix(struct pci_device *dev);

#endif /* !MSIX_H */
