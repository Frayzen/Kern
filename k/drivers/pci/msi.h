#ifndef MSI_H
#define MSI_H

#include "drivers/pci/pci.h"
#include "k/types.h"

#define MSI_MID 0x0
struct MSI_identifier {
	u8 capID;
	u8 nextPtr;
} __packed;

#define MSI_MC 0x2
struct MSI_msg_ctl {
	u8 enable : 1;
	u8 multi_msg_capable : 3;
	u8 multi_msg_enable : 3;
	u8 capable_64 : 1;
	u8 per_vector_mask_capable : 1;
	u8 __reserved0 : 7;
};

void enable_msi(struct pci_device *device);
void disable_msi(struct pci_device *device);

#endif /* !MSI_H */
