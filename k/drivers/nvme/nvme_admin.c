#include "nvme_admin.h"
#include "drivers/nvme/nvme.h"
#include "drivers/nvme/nvme_io.h"
#include "drivers/pci/cap.h"
#include "memalloc/memalloc.h"
#include <stdio.h>

volatile u32 *nvme_subm_doorbell(struct nvme_device *dev, u32 queue_id)
{
	return nvme_reg(
		dev, 0x1000 + (2 * queue_id * (4 << dev->capability_stride)));
}
volatile u32 *nvme_cmpl_doorbell(struct nvme_device *dev, u32 queue_id)
{
	return nvme_reg(dev, 0x1000 + ((2 * queue_id + 1) *
				       (4 << dev->capability_stride)));
}

int create_io_submission_queue(struct nvme_device *dev)
{
	// Create queue
	dev->io_subm_q.address = (u64)mmap();
	if (dev->io_subm_q.address == 0)
		return 0;
	dev->io_subm_q.size = SUBM_Q_SIZE - 1;
	dev->io_subm_q.door_bell = nvme_subm_doorbell(dev, 1);

	// Create command
	struct submission_q_entry cmd = {};
	cmd.cmd.opcode = OPCODE_IO_SUBMISSION_QUEUE_CREATE;
	// dword 6-7 = base address
	cmd.prp1 = dev->io_subm_q.address;
	// dword10
	u16 queue_id = 1;
	u32 queue_size = dev->io_subm_q.size;
	cmd.command_specific[0] = ((u16)queue_size << 16) | queue_id;
	// dword11
	u16 flags = FLAG_CONTIGUOUS_QUEUE;
	u32 completion_id = 1;
	cmd.command_specific[1] = ((u16)completion_id << 16) | flags;
	cmd.cmd.command_id = dev->next_command_id++;

	// Send commmand
	nvme_send_command(dev, &cmd, 1);
	return 1;
}

int create_io_completion_queue(struct nvme_device *dev)
{
	// Create queue
	dev->io_cmpl_q.address = (u64)mmap();
	if (dev->io_cmpl_q.address == 0)
		return 0;
	dev->io_cmpl_q.size = COMPL_Q_SIZE - 1;
	dev->io_cmpl_q.door_bell = nvme_cmpl_doorbell(dev, 1);

	struct submission_q_entry cmd = {};
	cmd.cmd.opcode = OPCODE_IO_COMPLETION_QUEUE_CREATE;

	// dword 6-7 = base address
	cmd.prp1 = dev->io_cmpl_q.address;

	// dword10
	u16 queue_id = 1;
	u32 queue_size = dev->io_cmpl_q.size;
	cmd.command_specific[0] = (queue_size << 16) | queue_id;

	// dword11
	u32 vector = 1;
	u32 flags = FLAG_CONTIGUOUS_QUEUE | FLAG_ENABLE_INTS;
	cmd.command_specific[1] = (vector << 16) | flags;

	cmd.cmd.command_id = dev->next_command_id++;

	nvme_send_command(dev, &cmd, 1);
	return 1;
}

int create_admin_submission_queue(struct nvme_device *dev)
{
	// Create queue
	dev->adm_subm_q.address = (u64)mmap();
	if (dev->adm_subm_q.address == 0)
		return 0;
	dev->adm_subm_q.size = SUBM_Q_SIZE - 1;
	dev->adm_subm_q.door_bell = nvme_subm_doorbell(dev, 0);
	// Write to the register
	*nvme_reg(dev, NVME_ASQ) = dev->adm_subm_q.address;
	CHECK_FATAL_STATUS(dev);
	return 1;
}

int create_admin_completion_queue(struct nvme_device *dev)
{
	// Create queue
	dev->adm_cmpl_q.address = (u64)mmap();
	if (dev->adm_cmpl_q.address == 0)
		return 0;
	dev->adm_cmpl_q.size = COMPL_Q_SIZE - 1;
	dev->adm_cmpl_q.door_bell = nvme_cmpl_doorbell(dev, 0);
	// Write to the register
	*nvme_reg(dev, NVME_ACQ) = dev->adm_cmpl_q.address;
	return 1;
}


int nvme_identify(struct nvme_device *device)
{
	u64 buffer = (u64)mmap();
	struct submission_q_entry cmd = {};
	cmd.cmd.opcode = 0x06;
	cmd.prp1 = buffer;
	cmd.cmd.command_id = device->next_command_id++;
  cmd.nsid = 1;
	nvme_send_command(device, &cmd, 1);
	return 1;
}
