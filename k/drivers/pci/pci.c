#include "drivers/pci/pci.h"
#include "drivers/pci/cap.h"
#include "drivers/pci/command.h"
#include "io.h"
#include "k/types.h"

u32 get_bar(struct pci_device *dev, u16 bar)
{
	disable_io(dev);
	u32 ret = pci_config_read(dev->bus, dev->slot, 0, bar);
	enable_io(dev);
	if (bar == PCI_BAR0)
		ret &= 0xFFFFFFF0;
	return ret;
}

// reads the 32 bits of the given register
u32 pci_config_read(u8 bus, u8 slot, u8 func, u8 offset)
{
	u32 address;
	u32 lbus = (u32)bus;
	u32 lslot = (u32)slot;
	u32 lfunc = (u32)func;
	u32 tmp = 0;

	// Create configuration address as per Figure 1
	address = (u32)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
			(offset & 0xFC) | ((u32)0x80000000));
	// Write out the address
	outl(CONFIG_ADDRESS, address);
	// Read in the data
	// (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
	tmp = (inl(CONFIG_DATA) >> ((offset & 2) * 8));
	return tmp;
}

// reads the 32 bits of the given register
void pci_config_write(u8 bus, u8 slot, u8 func, u8 offset, u32 val)
{
	u32 address;
	u32 lbus = (u32)bus;
	u32 lslot = (u32)slot;
	u32 lfunc = (u32)func;

	// Create configuration address as per Figure 1
	address = (u32)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
			(offset & 0xFC) | ((u32)0x80000000));
	// Write out the address
	outl(CONFIG_ADDRESS, address);
	// Read in the data
	// (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
	outl(CONFIG_DATA, val);
}

// reads the higher 16 bits of the given register
u16 pci_config_read_upper(u8 bus, u8 slot, u8 func, u8 offset)
{
	return (u16)(pci_config_read(bus, slot, func, offset) >> 16);
}

// reads the lower 16 bits of the given register
u16 pci_config_read_lower(u8 bus, u8 slot, u8 func, u8 offset)
{
	return (u16)(pci_config_read(bus, slot, func, offset) & 0xFFFF);
}

// returns non zero if updates the out value
int check_device(u8 bus, u8 slot, struct pci_device *out)
{
	u16 vendor, device;
	/* Try and read the first configuration register. Since there are no
     * vendors that == 0xFFFF, it must be a non-existent device. */
	if ((vendor = pci_config_read_lower(bus, slot, 0, PCI_VENDOR_ID)) != 0xFFFF) {
		device = pci_config_read_upper(bus, slot, 0, PCI_DEV_ID);
		u16 class = pci_config_read_upper(bus, slot, 0, PCI_CLASSES);

		out->bus = bus;
		out->slot = slot;
		out->vendorId = vendor;
		out->deviceId = device;
		out->classCode = class >> 8;
		out->subClass = class & 0xFF;
		out->headerType = pci_config_read_upper(bus, slot, 0, PCI_HEADER_TYPE) &
				  0xFF;
		out->status = pci_config_read_upper(bus, slot, 0, PCI_STATUS);
		check_capacities(out);
		return 1;
	}
	return 0;
}

// returns non zero if updates the out value
int look_for_device(u8 classCode, u8 subClass, struct pci_device *out)
{
	u16 bus;
	u8 device;

	for (bus = 0; bus < 256; bus++) {
		for (device = 0; device < 32; device++) {
			if (check_device(bus, device, out) &&
			    out->classCode == classCode &&
			    out->subClass == subClass)
				return 1;
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
		u16 cur = pci_config_read(dev->bus, dev->slot, 0, PCI_COMMAND);
    cur |= (1 << 1); // set the bit
    pci_config_write(dev->bus, dev->slot, 0, PCI_COMMAND, cur);
}

void enable_bus_master(struct pci_device *dev)
{
		u16 cur = pci_config_read(dev->bus, dev->slot, 0, PCI_COMMAND);
    cur |= (1 << 2); // set the bit
    pci_config_write(dev->bus, dev->slot, 0, PCI_COMMAND, cur);
}

void enable_interrupts(struct pci_device *dev)
{
		u16 cur = pci_config_read(dev->bus, dev->slot, 0, PCI_COMMAND);
    cur &= ~(1 << 10); // reset the bit 
    pci_config_write(dev->bus, dev->slot, 0, PCI_COMMAND, cur);
}
