#include "nvme.h"
#include "drivers/nvme/nvme_admin.h"
#include "drivers/nvme/nvme_io.h"
#include "drivers/pci/cap.h"
#include "drivers/pci/pci.h"
#include "assert.h"
#include "k/types.h"
#include "memalloc/memalloc.h"
#include <stdio.h>

volatile u32 *nvme_reg(struct nvme_device *dev, u32 offset)
{
	assert(dev->base_addr != 0x0);
	return (volatile u32 *)(dev->base_addr + offset);
}

void nvme_wait_status_ready(struct nvme_device *dev)
{
	while (!(*nvme_reg(dev, NVME_CST) & 0x1)) // check for 1 bit
		;
}

void get_version(struct nvme_device *device, u16 *major, u16 *minor, u16 *patch)
{
	u32 version = *nvme_reg(device, NVME_VS);
	// 32 bits are like so : major [31 - 16], minor [15: 8], patch[7: 0]
	*major = version >> 16;
	*minor = (version & 0xFFFF) >> 8;
	*patch = version & 0xFF;
}

void reset_controller(struct nvme_device *dev)
{
	printf("Resetting controller\n");
	struct nvme_controller_command cc = {};
	u32 *ccptr = (u32 *)&cc;
	*ccptr = *nvme_reg(dev, NVME_CC);

	cc.enable = 0;
	*nvme_reg(dev, NVME_CC) = *ccptr;
	while (*nvme_reg(dev, NVME_CST) & 0x1) // check for 1 bit
		;
	printf("Creating admin queues !\n");
	assert(create_admin_completion_queue(dev));
	assert(create_admin_submission_queue(dev));
	printf("Admin queue creaed\n");

	printf("Enabling controller\n");

	cc.io_subm_q_entry_size = SUBM_Q_SIZE_POW & 0xF;
	cc.io_compl_q_entry_size = COMPL_Q_SIZE_POW & 0xF;
	cc.mem_page_size = 0;
	cc.enable = 1;
	*nvme_reg(dev, NVME_CC) = *ccptr;
	nvme_wait_status_ready(dev);
	CHECK_FATAL_STATUS(dev);
}

void nvme_init(void)
{
	static struct nvme_device device = {};
	if (look_for_device(NVME_CLASS_CODE, NVME_SUBCLASS, &device.pci)) {
		printf("FOUND NVME AT %d %d with HEADER TYPE %d\n",
		       device.pci.bus, device.pci.slot, device.pci.headerType);
		assert(device.pci.headerType == 0x0);

		enable_interrupts(&device.pci);
		enable_bus_master(&device.pci);
		enable_mem_space(&device.pci);

		volatile u32 bar1 =
			(volatile u32)get_bar(&device.pci, PCI_BAR1);
		volatile u32 bar0 =
			(volatile u32)get_bar(&device.pci, PCI_BAR0);
		device.base_addr = (u64)(((u64)bar1 << 32) | (bar0 & ~(0xF)));
		// as soon as base addr is set, check status

		u16 major, minor, patch;
		get_version(&device, &major, &minor, &patch);

		if (device.pci.capabilities.msi_cap_offset)
			enable_msi(&device.pci);
		if (device.pci.capabilities.msix_cap_offset)
			enable_msix(&device.pci);

		// unmask the interrupts for all completion queues
		*nvme_reg(&device, NVME_INTMC) = 0xFFFFFFFF;

		// mask the interrupts for all completion queues
		/* *nvme_reg(&device, NVME_INTMS) = 0xFFFFFFFF; */

		device.capability_stride =
			(*nvme_reg(&device, NVME_CAP + 0x4)) & 0xF;
		assert(device.capability_stride ==
		       ((device.base_addr >> 12) & 0xF));

		/* printf("Max queue entries supported (MQES): %d\n", */
		/*        nvme_read_reg(&device, NVME_CAP) & 0xFFFF); */

		reset_controller(&device);

		printf("Identify...\n");
		assert(nvme_identify(&device));
		printf("[DONE]\n");

		printf("Creating IO queues !\n");
		assert(create_io_completion_queue(&device));
		assert(create_io_submission_queue(&device));

		char *buffer = (char *)mmap();
		printf("READING ...\n");
		nvme_read(&device, 1, 1, buffer);
		printf("READ:\n");
		for (int i = 0; i < 10; i++) {
			printf("%c ", buffer[i]);
		}

		printf("NVME SETUP DONE\n");
	} else
		printf("NO NVME FOUND\n");
}
