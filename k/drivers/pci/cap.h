#ifndef CAP_H
#define CAP_H

#include "k/compiler.h"
#include "k/types.h"

struct pci_device;

struct pci_capabilites {
	u8 msi_cap_offset;
	u8 msix_cap_offset;
	u8 power_cap_offset;
} __packed;

#define CAP_REG 0x34

#define POW_CAP_ID 0x1
#define SMI_CAP_ID 0x05
#define SMIX_CAP_ID 0x11

void check_capacities(struct pci_device *device);

#endif /* !CAP_H */
