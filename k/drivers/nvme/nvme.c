#include "nvme.h"
#include "drivers/pci/cap.h"
#include "drivers/pci/pci.h"
#include "assert.h"
#include "k/types.h"
#include "memalloc/memalloc.h"
#include "nvme_utils.h"
#include <stdio.h>
#include <string.h>

#define CHECK_FATAL_STATUS(Device) \
	assert(!(nvme_read_reg(Device, NVME_CST) & 0x2))

void wait_status_ready(struct nvme_device *dev)
{
	while (!(nvme_read_reg(dev, NVME_CST) & 0x1)) // check for 1 bit
		;
}

struct nvme_command_entry create_io_command(struct nvme_device *dev, u8 opcode,
					    u8 nsid, void *data, u64 lba,
					    u16 num_blocks)
{
	struct nvme_command_entry command_entry = {};
	command_entry.opcode = opcode;
	command_entry.nsid = nsid;
	command_entry.prp1 = (u32)data;
	command_entry.prp2 = 0;
	command_entry.command_id = dev->next_command_id++;
	command_entry.command_specific[0] = (u32)lba;
	command_entry.command_specific[1] = (u32)((u64)lba >> 32);
	command_entry.command_specific[2] = (u16)(num_blocks - 1);
	return command_entry;
}

int nvme_send_command(struct nvme_device *device,
		      struct nvme_command_entry *command_entry)
{
	memcpy(get_subm_tail(device), command_entry,
	       sizeof(struct nvme_command_entry));
	device->s_tail++;

	wait_status_ready(device);
	nvme_write_reg(device, NVME_SQxTDBL(1, device->capability_stride),
		       device->s_tail);
	__asm__ volatile("mfence"); // Force write completion
	if (device->s_tail == device->io_subm_q.size)
		device->s_tail = 0;
	CHECK_FATAL_STATUS(device);
	printf("Command %d happened correctly\n", command_entry->command_id);
	return 1;
}

int nvme_rcv_command(struct nvme_device *device)
{
	if (!(get_comp_head(device)->phase_bit)) {
		return 0; // No new completions
	}
	// Completion aknowledged, reset the bit
	get_comp_head(device)->phase_bit = 0;
	printf("Got status 0x%x\n", get_comp_head(device)->status);
	assert(get_comp_head(device)->status == 0);

	device->c_head++;
	nvme_write_reg(device, NVME_CQxHDBL(1, device->capability_stride),
		       device->c_head);
	__asm__ volatile("mfence"); // Force write completion
	if (device->c_head == device->io_cmpl_q.size)
		device->c_head = 0;
	return 1;
}

int create_io_submission_queue(struct nvme_device *dev)
{
	dev->io_subm_q.address = (u32)mmap();
	dev->s_tail = 0;
	dev->io_subm_q.size = QUEUE_SIZE;
	if (dev->io_subm_q.address == 0)
		return 0;

	struct nvme_command_entry sub_queue_create_cmd = {};
	sub_queue_create_cmd.opcode = OPCODE_IO_SUBMISSION_QUEUE_CREATE;

	// dword 6-7 = base address
	sub_queue_create_cmd.prp1 = dev->io_subm_q.address;

	// dword10
	u8 queue_id = 1;
	u8 queue_size = dev->io_subm_q.size - 1;
	sub_queue_create_cmd.command_specific[0] =
		((u16)queue_size << 8) + queue_id;

	// dword11
	u8 flags = FLAG_CONTIGUOUS_QUEUE;
	u8 completion_id = 1;
	sub_queue_create_cmd.command_specific[1] =
		((u16)completion_id << 8) + flags;

	sub_queue_create_cmd.command_id = dev->next_command_id++;

	assert(nvme_send_command(dev, &sub_queue_create_cmd));
	return 1;
}

int create_io_completion_queue(struct nvme_device *dev)
{
	dev->io_cmpl_q.address = (u32)mmap();
	dev->c_head = 0;
	dev->io_cmpl_q.size = QUEUE_SIZE;
	if (dev->io_cmpl_q.address == 0)
		return 0;

	struct nvme_command_entry compl_queue_create_cmd = {};
	compl_queue_create_cmd.opcode = OPCODE_IO_COMPLETION_QUEUE_CREATE;

	// dword 6-7 = base address
	compl_queue_create_cmd.prp1 = dev->io_cmpl_q.address;

	// dword10
	u16 queue_id = 1;
	u16 queue_size = dev->io_cmpl_q.size - 1;
	compl_queue_create_cmd.command_specific[0] =
		(queue_size << 16) + queue_id;

	// dword11
	u16 flags = FLAG_CONTIGUOUS_QUEUE | FLAG_ENABLE_INTS;
	u16 interrupt_vector = 1;
	compl_queue_create_cmd.command_specific[1] =
		((u16)interrupt_vector << 16) + flags;

	compl_queue_create_cmd.command_id = dev->next_command_id++;

	assert(nvme_send_command(dev, &compl_queue_create_cmd));
	return 1;
}

int create_admin_submission_queue(struct nvme_device *dev)
{
	dev->adm_subm_q.address = (u64)mmap();
	if (dev->adm_subm_q.address == 0)
		return 0;
	dev->adm_subm_q.size = QUEUE_SIZE - 1;
	nvme_write_reg(dev, NVME_ASQ, dev->adm_subm_q.address);
	CHECK_FATAL_STATUS(dev);
	return 1;
}

int create_admin_completion_queue(struct nvme_device *dev)
{
	dev->adm_cmpl_q.address = (u64)mmap();
	if (dev->adm_cmpl_q.address == 0)
		return 0;
	dev->adm_cmpl_q.size = QUEUE_SIZE - 1;
	nvme_write_reg(dev, NVME_ACQ, dev->adm_cmpl_q.address);
	return 1;
}

static u32 nsid = 0;

// 0 if everything is fine
int nvme_read(struct nvme_device *device, u64 lba, u32 sector_count,
	      void *buffer)
{
	struct nvme_command_entry command = create_io_command(
		device, 0x02, nsid, buffer, lba, sector_count);
	if (!nvme_send_command(device, &command))
		return 0;
  CHECK_FATAL_STATUS(device);
  printf("PTR IS 0x%x\n", get_comp_head(device));
	while (!nvme_rcv_command(device)) {
	} // Wait for completion
	return 1;
}

int nvme_write(struct nvme_device *device, u64 lba, u32 sector_count,
	       void *buffer)
{
	struct nvme_command_entry command = create_io_command(
		device, 0x01, nsid, buffer, lba, sector_count);
	if (!nvme_send_command(device, &command))
		return 0;
	return 1;
}

void get_version(struct nvme_device *device, u16 *major, u16 *minor, u16 *patch)
{
	u32 version = nvme_read_reg(device, NVME_VS);
	// 32 bits are like so : major [31 - 16], minor [15: 8], patch[7: 0]
	*major = version >> 16;
	*minor = (version & 0xFFFF) >> 8;
	*patch = version & 0xFF;
}

void print_chead(char *c)
{
	printf("START IS %p\n", c);
	for (u32 i = 0; i < sizeof(struct nvme_completion); i++) {
		printf("Byte %d : 0x%x\n", i, c[i]);
	}
}

void init_nvme(void)
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
		device.base_addr = (u64)(((u64)bar1 << 32) | bar0);
		// as soon as base addr is set, check status

		u16 major, minor, patch;
		get_version(&device, &major, &minor, &patch);
		printf("Nvme version is %d.%d.%d\n", major, minor, patch);

		if (device.pci.capabilities.msi_cap_offset)
			enable_msi(&device.pci);
		if (device.pci.capabilities.msix_cap_offset)
			enable_msix(&device.pci);
		// unmask the interrupts for all completion queues
		nvme_write_reg(&device, NVME_INTMC, 0xFFFFFFFF);

		device.capability_stride =
			(nvme_read_reg(&device, NVME_CAP + 0x4)) & 0xF;
		printf("Capability stride is %d\n", device.capability_stride);

		/* printf("Max queue entries supported (MQES): %d\n", */
		/*        nvme_read_reg(&device, NVME_CAP) & 0xFFFF); */

		printf("Resetting controller\n");
		nvme_write_reg(&device, NVME_CC,
			       nvme_read_reg(&device, NVME_CC) & ~(0x1));

		printf("Creating admin queues !\n");
		assert(create_admin_completion_queue(&device));
		assert(create_admin_submission_queue(&device));
		printf("Admin queue creatx /1gx 0xfebf0000ed\n");

		printf("Enabling controller\n");
		u32 cc = 0;
		cc |= 1; // Enable device
		cc |= ((QUEUE_SIZE_POW) & 0xF) << 20; // IOCQES
		cc |= ((QUEUE_SIZE_POW) & 0xF) << 16; // IOSQES
		nvme_write_reg(&device, NVME_CC, cc);
		CHECK_FATAL_STATUS(&device);

		printf("Creating IO queues !\n");
		assert(create_io_completion_queue(&device));
		char *c = (char *)get_comp_head(&device);
		print_chead(c);
		assert(create_io_submission_queue(&device));
		print_chead(c);
		printf("IO queues created\n");

		char *buffer = mmap();

		while (nvme_rcv_command(&device)) {
			printf("Clearing ...\n");
		} // Wait for completion
		print_chead(c);
		printf("READING ...\n");
		assert(nvme_read(&device, 5, 1, buffer));

		printf("NVME SETUP DONE\n");
	} else
		printf("NO NVME FOUND\n");
}
