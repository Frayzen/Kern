#include "nvme.h"
#include "drivers/nvme/nvme_admin.h"
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

static u32 nsid = 0;

/* // 0 if everything is fine */
/* int nvme_read(struct nvme_device *device, u64 lba, u32 sector_count, */
/* 	      void *buffer) */
/* { */
/* 	struct nvme_command_entry command = create_io_command( */
/* 		device, 0x02, nsid, buffer, lba, sector_count); */
/* 	if (!nvme_send_command(device, &command)) */
/* 		return 0; */
/*   CHECK_FATAL_STATUS(device); */
/*   printf("PTR IS 0x%x\n", get_comp_head(device)); */
/* 	while (!nvme_rcv_command(device)) { */
/* 	} // Wait for completion */
/* 	return 1; */
/* } */

/* int nvme_write(struct nvme_device *device, u64 lba, u32 sector_count, */
/* 	       void *buffer) */
/* { */
/* 	struct nvme_command_entry command = create_io_command( */
/* 		device, 0x01, nsid, buffer, lba, sector_count); */
/* 	if (!nvme_send_command(device, &command)) */
/* 		return 0; */
/* 	return 1; */
/* } */

void get_version(struct nvme_device *device, u16 *major, u16 *minor, u16 *patch)
{
	u32 version = *nvme_reg(device, NVME_VS);
	// 32 bits are like so : major [31 - 16], minor [15: 8], patch[7: 0]
	*major = version >> 16;
	*minor = (version & 0xFFFF) >> 8;
	*patch = version & 0xFF;
}

void print_chead(struct nvme_queue *q)
{
	char *c = (char *)q->address;
	printf("START IS %p\n", c);
	for (u32 i = 0; i < sizeof(struct completion_q_entry); i++) {
		printf("Byte %d : 0x%x\n", i, c[i]);
	}
}

void reset_controller(struct nvme_device* dev)
{

		printf("Resetting controller\n");
    volatile struct nvme_controller_command* cc = (volatile void*) nvme_reg(dev, NVME_CC);
    cc->enable = 0;
		while (*nvme_reg(dev, NVME_CST) & 0x1) // check for 1 bit
			;

		printf("Creating admin queues !\n");
		assert(create_admin_completion_queue(dev));
		assert(create_admin_submission_queue(dev));
		printf("Admin queue creaed\n");

		printf("Enabling controller\n");

    cc->io_subm_q_entry_size = QUEUE_SIZE_POW & 0xF;
    cc->io_compl_q_entry_size = QUEUE_SIZE_POW & 0xF;
    cc->enable = 1;
		CHECK_FATAL_STATUS(dev);
}

void nvme_init(void)
{
	struct nvme_device device = {};
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
		printf("Nvme version is %d.%d.%d\n", major, minor, patch);

		if (device.pci.capabilities.msi_cap_offset)
			enable_msi(&device.pci);
		if (device.pci.capabilities.msix_cap_offset)
			enable_msix(&device.pci);
		// unmask the interrupts for all completion queues
		*nvme_reg(&device, NVME_INTMC) = 0xFFFFFFFF;

		device.capability_stride =
			(*nvme_reg(&device, NVME_CAP + 0x4)) & 0xF;
		assert(device.capability_stride ==
		       ((device.base_addr >> 12) & 0xF));
		printf("Capability stride is %d\n", device.capability_stride);

		/* printf("Max queue entries supported (MQES): %d\n", */
		/*        nvme_read_reg(&device, NVME_CAP) & 0xFFFF); */

    reset_controller(&device);

		printf("Creating IO queues !\n");
		assert(create_io_completion_queue(&device));
		print_chead(&device.adm_cmpl_q);
		assert(create_io_submission_queue(&device));
		print_chead(&device.adm_cmpl_q);
		printf("IO queues created\n");

		/* char *buffer = mmap(); */
		/* printf("READING ...\n"); */
		/* assert(*nvme_reg(&device, 5, 1, buffer)); */

		printf("NVME SETUP DONE\n");
	} else
		printf("NO NVME FOUND\n");
}
