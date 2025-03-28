#ifndef PCI_H
#define PCI_H

/*
Offset |Bits 31-24 | Bits 23-16  | Bits 15-8    | Bits 7-0 
=============================================================
0x0    |      Device ID          |         Vendor ID
0x4 	 |      Status             |          Command
0x8 	 | Class code | Subclass 	 | Prog IF 	    | Revision ID
0xC 	 |    BIST 	  | Header Type| Latency Timer| Cache Line Size

/!\ The lower addresses contain the least significant portions of the field
*/

#define PCI_VENDOR_ID 0x0
#define PCI_DEV_ID 0x2

#define PCI_COMMAND 0x4
#define PCI_STATUS 0x6

#define PCI_CLASSES 0xA

#define PCI_HEADER_TYPE 0xE

#include "drivers/pci/cap.h"
#include "k/compiler.h"
#include "k/types.h"
#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

struct pci_device {
	u8 bus;
	u8 slot;
	u16 deviceId;
	u16 vendorId;
	u16 status;
	// might add command
	u8 classCode;
	u8 subClass;
	// ...
	u8 headerType;
	// ...
	struct pci_capabilites capabilities;
} __packed;

#define PCI_STATUS_CAPABILITY_LIST_FLAG (1 << 4)

#define PCI_BAR0 0x10
#define PCI_BAR1 0x14

u32 pci_readl(u8 bus, u8 slot, u8 func, u8 offset);
u16 pci_readw(u8 bus, u8 slot, u8 func, u8 offset);
void pci_writel(u8 bus, u8 slot, u8 func, u8 offset, u32 val);
void pci_writew(u8 bus, u8 slot, u8 func, u8 offset, u16 val);

#define pcidev_readl(Device, Offset) \
	pci_readl((Device)->bus, (Device)->slot, 0, (Offset))
#define pcidev_readw(Device, Offset) \
	pci_readw((Device)->bus, (Device)->slot, 0, (Offset))

#define pcidev_writel(Device, Offset, Val) \
	pci_writel((Device)->bus, (Device)->slot, 0, (Offset), (Val))
#define pcidev_writew(Device, Offset, Val) \
	pci_writew((Device)->bus, (Device)->slot, 0, (Offset), (Val))

u32 get_bar(struct pci_device *dev, u16 bar);

// returns non zero if found, updates the out value accordingly
int look_for_device(u8 classCode, u8 subClass, struct pci_device *out);

void enable_mem_space(struct pci_device *dev);
void enable_bus_master(struct pci_device *dev);
void enable_interrupts(struct pci_device *dev);

#endif /* !PCI_H */
