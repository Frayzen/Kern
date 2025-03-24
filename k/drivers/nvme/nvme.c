#include "nvme.h"
#include "drivers/pci/cap.h"
#include "drivers/pci/pci.h"
#include "assert.h"
#include "k/types.h"
#include "memalloc/memalloc.h"
#include "nvme_utils.h"
#include <stdio.h>
#include <string.h>

struct nvme_command_entry create_io_command(struct nvme_device* dev, u8 opcode, u8 nsid, void *data,
					    u64 lba, u16 num_blocks)
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
	u64 sq_entry_addr =
		device->submission_q.address +
		(device->s_tail * sizeof(struct nvme_command_entry));

	memcpy((void *)sq_entry_addr, command_entry,
	       sizeof(struct nvme_command_entry));
	device->s_tail++;
	nvme_write_reg(device, NVME_SQxTDBL(1, device->capability_stride),
		       device->s_tail);
	__asm__ volatile("mfence"); // Force write completion
	if (device->s_tail == QUEUE_SIZE)
		device->s_tail = 0;
	return 1;
}

int nvme_rcv_command(struct nvme_device *device)
{
	u64 cq_entry_addr = device->completion_q.address +
			    (device->c_head * sizeof(struct nvme_completion));
	struct nvme_completion *completion = (void *)cq_entry_addr;
	if (!completion->phase_bit) {
		return 0; // No new completions
	}
	device->c_head++;
	nvme_write_reg(device, NVME_CQxHDBL(1, device->capability_stride),
		       device->c_head);
	__asm__ volatile("mfence"); // Force write completion
	if (device->c_head == QUEUE_SIZE)
		device->c_head = 0;
	return 1;
}

int create_io_submission_queue(struct nvme_device *dev)
{
	u32 base_address = (u32)mmap();
	if (base_address == 0)
		return 0;

	struct nvme_command_entry sub_queue_create_cmd = {};
	sub_queue_create_cmd.opcode = OPCODE_IO_SUBMISSION_QUEUE_CREATE;

	// dword 6-7 = base address
	sub_queue_create_cmd.prp1 = base_address;

	// dword10
	u8 queue_id = 1;
	u8 queue_size = QUEUE_SIZE - 1;
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
	u32 base_address = (u32)mmap();
	if (base_address == 0)
		return 0;

	struct nvme_command_entry compl_queue_create_cmd = {};
	compl_queue_create_cmd.opcode = OPCODE_IO_COMPLETION_QUEUE_CREATE;

	// dword 6-7 = base address
	compl_queue_create_cmd.prp1 = base_address;

	// dword10
	u8 queue_id = 1;
	u8 queue_size = QUEUE_SIZE - 1;
	compl_queue_create_cmd.command_specific[0] =
		((u16)queue_size << 8) + queue_id;

	// dword11
	u8 flags = FLAG_CONTIGUOUS_QUEUE | FLAG_ENABLE_INTS;
	u8 interrupt_vector = 25;
	compl_queue_create_cmd.command_specific[1] =
		((u16)interrupt_vector << 8) + flags;

  compl_queue_create_cmd.command_id = dev->next_command_id++;

	assert(nvme_send_command(dev, &compl_queue_create_cmd));
	return 1;
}

int create_admin_submission_queue(struct nvme_device *dev)
{
	dev->submission_q.address = (u64)mmap();
	if (dev->submission_q.address == 0)
		return 0;
	dev->submission_q.size = QUEUE_SIZE - 1;
	nvme_write_reg(dev, NVME_ASQ_REG, dev->submission_q.address);
	return 1;
}

int create_admin_completion_queue(struct nvme_device *dev)
{
	dev->completion_q.address = (u64)mmap();
	if (dev->completion_q.address == 0)
		return 0;
	dev->completion_q.size = QUEUE_SIZE - 1;
	nvme_write_reg(dev, NVME_ACQ_REG, dev->completion_q.address);
	return 1;
}

static u32 nsid = 0;

// 0 if everything is fine
int nvme_read(struct nvme_device *device, u64 lba, u32 sector_count,
	      void *buffer)
{
	struct nvme_command_entry command =
		create_io_command(device, 0x02, nsid, buffer, lba, sector_count);
	if (!nvme_send_command(device, &command))
		return 0;
	printf("Polling ...\n");
	while (!nvme_rcv_command(device)) {
	} // Wait for completion
	return 1;
}

int nvme_write(struct nvme_device *device, u64 lba, u32 sector_count,
	       void *buffer)
{
	struct nvme_command_entry command =
		create_io_command(device, 0x01, nsid, buffer, lba, sector_count);
	if (!nvme_send_command(device, &command))
		return 0;
	return 1;
}

void init_nvme(void)
{
	struct nvme_device device;
	if (look_for_device(NVME_CLASS_CODE, NVME_SUBCLASS, &device.pci)) {
		printf("FOUND NVME AT %d %d with HEADER TYPE %d\n",
		       device.pci.bus, device.pci.slot, device.pci.headerType);
		assert(device.pci.headerType == 0x0);

		if (device.pci.capabilities.msi_cap_offset)
			enable_msi(&device.pci);

		if (device.pci.capabilities.msix_cap_offset)
			enable_msix(&device.pci);

		volatile u32 bar1 =
			(volatile u32)get_bar(&device.pci, PCI_BAR1);
		volatile u32 bar0 =
			(volatile u32)get_bar(&device.pci, PCI_BAR0);

		device.base_addr = (u64)(((u64)bar1 << 32) | bar0);
		device.capability_stride = (device.base_addr >> 12) & 0xF;

		assert(create_admin_submission_queue(&device));
		assert(create_admin_completion_queue(&device));
		assert(create_io_submission_queue(&device));
		assert(create_io_completion_queue(&device));

		char *buffer = mmap();

		while (nvme_rcv_command(&device)) {
			printf("Clearing ...\n");
		} // Wait for completion
		printf("READING ...\n");
		assert(nvme_read(&device, 0, 1, buffer));

		printf("NVME SETUP DONE\n");
	} else
		printf("NO NVME FOUND\n");
}
