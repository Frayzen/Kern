#include "drivers/pci/pci.h"
#include "drivers/pci/cap.h"
#include "drivers/pci/command.h"
#include "io.h"
#include "k/types.h"

u32 get_bar(struct pci_device *dev, u16 bar)
{
	disable_io(dev);
	u32 ret = (pcidev_readw(dev, bar + 0x2) << 16) |
		  pcidev_readw(dev, bar);
	enable_io(dev);
	if (bar == PCI_BAR0)
		ret &= 0xFFFFFFF0;
	return ret;
}

static void set_addr(u8 bus, u8 slot, u8 func, u8 offset)
{
	u32 address = 0;
	u32 lbus = (u32)bus;
	u32 lslot = (u32)slot;
	u32 lfunc = (u32)func;
	// Create configuration address as per Figure 1
	address = (u32)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
			(offset & 0xFC) | ((u32)0x80000000));
	// Write out the address
	outl(0xCF8, address);
}

u32 pci_readl(u8 bus, u8 slot, u8 func, u8 offset)
{
	set_addr(bus, slot, func, offset);
	return inl(CONFIG_DATA);
}

u16 pci_readw(u8 bus, u8 slot, u8 func, u8 offset)
{
  set_addr(bus, slot, func, offset);
	return (u16)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}

void pci_writel(u8 bus, u8 slot, u8 func, u8 offset, u32 val)
{
	set_addr(bus, slot, func, offset);
	outl(CONFIG_DATA, val);
}

void pci_writew(u8 bus, u8 slot, u8 func, u8 offset, u16 val)
{
	set_addr(bus, slot, func, offset);
	outw(CONFIG_DATA, val);
}

// returns non zero if updates the out value
int check_device(u8 bus, u8 slot, struct pci_device *out)
{
	u16 vendor, device = 0;
	/* Try and read the first configuration register. Since there are no
     * vendors that == 0xFFFF, it must be a non-existent device. */
	if ((vendor = pci_readw(bus, slot, 0, PCI_VENDOR_ID))) {
		device = pci_readw(bus, slot, 0, PCI_DEV_ID);
		u16 class = pci_readw(bus, slot, 0, 0xA);

		out->bus = bus;
		out->slot = slot;
		out->vendorId = vendor;
		out->deviceId = device;
		out->classCode = class >> 8;
		out->subClass = class & 0xFF;
		out->headerType = pci_readw(bus, slot, 0, PCI_HEADER_TYPE) &
				  0xFF;
		out->status = pci_readw(bus, slot, 0, PCI_STATUS);
		check_capacities(out);
		return 1;
	}
	return 0;
}

// returns non zero if updates the out value
int look_for_device(u8 classCode, u8 subClass, struct pci_device *out)
{
	u16 bus = 0;
	u8 device = 0;

	for (bus = 0; bus < 256; bus++) {
		for (device = 0; device < 32; device++) {
			if (check_device(bus, device, out) &&
			    out->classCode == classCode &&
			    out->subClass == subClass) {
				return 1;
			}
		}
	}
	return 0;
}

/*
COMMAND REGISTER BITS 
10 Interrupt Disable
9 Fast Back-to-Back Enable
8 SERR# Enable
7 Reserved 
6 Parity Error Response
5 VGA Palette Snoop
4 Memory Write and Invalidate Enable
3 Special Cycles
2 Bus Master
1 Memory Space
0 I/O Space
 */

void enable_mem_space(struct pci_device *dev)
{
	u16 cur = pcidev_readw(dev, PCI_COMMAND);
	cur |= (1 << 1); // set the bit
	pcidev_writew(dev, PCI_COMMAND, cur);
}

void enable_bus_master(struct pci_device *dev)
{
	u16 cur = pcidev_readw(dev, PCI_COMMAND);
	cur |= (1 << 2); // set the bit
	pcidev_writew(dev, PCI_COMMAND, cur);
}

void enable_interrupts(struct pci_device *dev)
{
	u16 cur = pcidev_readw(dev, PCI_COMMAND);
	cur &= ~(1 << 10); // reset the bit
	pcidev_writew(dev, PCI_COMMAND, cur);
}
