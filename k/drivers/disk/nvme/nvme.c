#include "nvme.h"
#include "drivers/disk/nvme/nvme_admin.h"
#include "drivers/disk/nvme/nvme_io.h"
#include "drivers/pci/cap.h"
#include "drivers/pci/msi.h"
#include "drivers/pci/msix.h"
#include "drivers/pci/pci.h"
#include "assert.h"
#include "k/types.h"
#include "memalloc/memalloc.h"
#include <stdio.h>

struct nvme_device device = {};
struct nvme_device *nvme_dev = &device;

volatile u32 *nvme_reg(u32 offset)
{
	assert(nvme_dev->base_addr != 0x0);
	return (volatile u32 *)(nvme_dev->base_addr + offset);
}

void nvme_wait_status_ready()
{
	while (!(*nvme_reg(NVME_CST) & 0x1)) // check for 1 bit
		;
}

void get_version(u16 *major, u16 *minor, u16 *patch)
{
	u32 version = *nvme_reg(NVME_VS);
	// 32 bits are like so : major [31 - 16], minor [15: 8], patch[7: 0]
	*major = version >> 16;
	*minor = (version & 0xFFFF) >> 8;
	*patch = version & 0xFF;
}

void reset_controller()
{
	printf("Resetting controller\n");
	struct nvme_controller_command cc = {};
	u32 *ccptr = (u32 *)&cc;
	*ccptr = *nvme_reg(NVME_CC);

	cc.enable = 0;
	*nvme_reg(NVME_CC) = *ccptr;
	while (*nvme_reg(NVME_CST) & 0x1) // check for 1 bit
		;
	printf("Creating admin queues !\n");
	assert(create_admin_completion_queue());
	assert(create_admin_submission_queue());
	printf("Admin queue created\n");

	printf("Enabling controller\n");

	cc.io_subm_q_entry_size = SUBM_Q_SIZE_POW & 0xF;
	cc.io_compl_q_entry_size = COMPL_Q_SIZE_POW & 0xF;
	cc.mem_page_size = 0;
	cc.enable = 1;
	*nvme_reg(NVME_CC) = *ccptr;
	nvme_wait_status_ready();
	NVME_CHECK_STATUS;
}

void nvme_init(void)
{
	if (look_for_device(NVME_CLASS_CODE, NVME_SUBCLASS, &device.pci)) {
		printf("FOUND NVME AT %d %d with HEADER TYPE %d\n",
		       device.pci.bus, device.pci.slot, device.pci.headerType);
		assert(device.pci.headerType == 0x0);

		set_interrupts(&device.pci, 1);
		set_bus_master(&device.pci, 1);
		set_mem_space(&device.pci, 1);

		u64 bar1 = get_bar(&device.pci, PCI_BAR1);
		u64 bar0 = get_bar(&device.pci, PCI_BAR0) & 0xFFFFFFF0;
		device.base_addr = (bar1 << 32) | bar0;
		// as soon as base addr is set, check status

		u16 major, minor, patch;
		get_version(&major, &minor, &patch);

		if (device.pci.capabilities.msix_cap_offset) {
			if (device.pci.capabilities.msi_cap_offset)
				disable_msi(&device.pci);
			enable_msix(&device.pci);
		} else if (device.pci.capabilities.msi_cap_offset)
			enable_msi(&device.pci);

		device.capability_stride = (*nvme_reg(NVME_CAP + 0x4)) & 0xF;
		assert(device.capability_stride ==
		       ((device.base_addr >> 12) & 0xF));

		/* printf("Max queue entries supported (MQES): %d\n", */
		/*        nvme_read_reg(&device, NVME_CAP) & 0xFFFF); */

		reset_controller();

		printf("Identify...\n");
		assert(nvme_identify());
		printf("[DONE]\n");

		printf("Creating IO queues !\n");
		assert(create_io_completion_queue());
		assert(create_io_submission_queue());

		nvme_wait_status_ready();
		printf("NVME SETUP DONE\n");
	} else
		printf("NO NVME FOUND\n");
}
