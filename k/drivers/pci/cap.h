#ifndef CAP_H
#define CAP_H

#include "k/compiler.h"
#include "k/types.h"

struct pci_device;

struct pci_capabilites {
	u8 msi_cap_offset;
	u8 msix_cap_offset;
} __packed;

struct MSI_capability {
	u8 capID;
	u8 nextPtr;
	u8 enable : 1;
	u8 __reserved0 : 7;
	u16 __reserved1;
} __packed;

struct MSIX_capability {
	u8 capID;
	u8 nextPtr;
	u16 table_size : 11;
	u8 __reserved0 : 3;
	u8 function_mask : 1;
	u8 enable : 1;
} __packed;

struct MSIX_vector_table {
	u32 __reserved0 : 31;
	u8 masked : 1;
	u32 msg_data;
	u64 msg_addr; // mask the low bits (& 0xFFFFFFF0)
} __packed;

#define CAP_REG 0x34

#define SMI_CAP_ID 0x05
#define SMIX_CAP_ID 0x11

void check_capacities(struct pci_device *device);

void enable_msi(struct pci_device *device);
void enable_msix(struct pci_device *device);

#endif /* !CAP_H */
