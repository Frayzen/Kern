#include "drivers/pci/pci.h"
#include "io.h"
#include "k/types.h"
#include <stdio.h>

u32 pciConfigRead(u8 bus, u8 slot, u8 func, u8 offset)
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

u16 pciConfigReadUpper(u8 bus, u8 slot, u8 func, u8 offset)
{
	return (u16)(pciConfigRead(bus, slot, func, offset) >> 16);
}

u16 pciConfigReadLower(u8 bus, u8 slot, u8 func, u8 offset)
{
	return (u16)(pciConfigRead(bus, slot, func, offset) & 0xFFFF);
}

u16 checkDevice(u8 bus, u8 slot)
{
	u16 vendor, device;
	/* Try and read the first configuration register. Since there are no
     * vendors that == 0xFFFF, it must be a non-existent device. */
	if ((vendor = pciConfigReadLower(bus, slot, 0, 0)) != 0xFFFF) {
		/* printf("Found vendor id %d\n", vendor); */
		device = pciConfigReadUpper(bus, slot, 0, 2);
		/* printf("Found device id %d\n", device); */
		u16 class = pciConfigReadUpper(bus, slot, 0, 0x8);
		u8 classCode = class >> 8;
		u8 subClass = class & 0xFF;

		if (classCode == 0x1 && subClass == 0x8) {
			printf("NVME Device found (class %x sub %x)\n",
			       classCode, subClass);
		} else
      vendor = 0;
	}
	return (vendor);
}

void checkAllBuses(void)
{
	u16 bus;
	u8 device;

	for (bus = 0; bus < 256; bus++) {
		for (device = 0; device < 32; device++) {
			checkDevice(bus, device);
		}
	}
}
