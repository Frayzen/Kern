#include "nvme_io.h"
#include "memalloc/memalloc.h"
#include "nvme.h"
#include "k/types.h"
#include <stdio.h>
void *get_queue_ptr(struct nvme_queue *q, u8 is_submission)
{
	u32 offset = is_submission ? sizeof(struct submission_q_entry) :
				     sizeof(struct completion_q_entry);
	return (void *)(q->address + offset * q->ptr);
}

void nvme_send_command(struct nvme_device *device,
		       struct submission_q_entry *cmd, u8 is_admin)
{
	struct nvme_queue *sub_queue = is_admin ? (&device->adm_subm_q) :
						  &device->io_subm_q;
	struct nvme_queue *compl_queue = is_admin ? (&device->adm_cmpl_q) :
						    (&device->io_cmpl_q);

	struct submission_q_entry *subm_tail = get_queue_ptr(sub_queue, 1);
	volatile struct completion_q_entry *compl_head = get_queue_ptr(compl_queue, 0);

	*subm_tail = *cmd;

	*(sub_queue->door_bell) = ++(sub_queue->ptr);
	nvme_wait_status_ready(device);
	if (sub_queue->ptr == sub_queue->size)
		sub_queue->ptr = 0;
	CHECK_FATAL_STATUS(device);

	printf("Command 0x%x sent correctly\n", cmd->cmd.command_id);
	while (!compl_head->phase_bit)
		continue;
	if (compl_head->status != 0)
		printf("Error while processing command\n");
	else
		printf("Command %d happened correctly\n", cmd->cmd.command_id);

	// Completion aknowledged, reset the bit
	compl_head->phase_bit = 0;
	/* assert(compl_head->status == 0); */

	*compl_queue->door_bell = ++(compl_queue->ptr);
	if (compl_queue->ptr == 0)
		compl_queue->ptr = 0;
}

int nvme_read(struct nvme_device *dev, u64 lba, u32 sector_count, void *buffer)
{
	struct submission_q_entry cmd = {};

  u64 buf = (u64) buffer;
	cmd.cmd.opcode = 0x02;
  cmd.nsid = 1;
	cmd.command_specific[0] = (u32)lba;
	cmd.command_specific[1] = (u32)(lba >> 32);
	cmd.command_specific[2] = sector_count & 0xFFFF;
	cmd.prp1 = buf;

	cmd.cmd.command_id = dev->next_command_id++;
	nvme_send_command(dev, &cmd, 0);
	return 1;
}

int nvme_write(struct nvme_device *device, u64 lba, u32 sector_count,
	       void *buffer)
{
	struct submission_q_entry cmd =
		create_io_command(device, 0x01, 0, buffer, lba, sector_count);
	nvme_send_command(device, &cmd, 0);
	return 1;
}

int nvme_identify(struct nvme_device *device)
{
	u64 buffer = (u64)mmap();
	struct submission_q_entry cmd = {};
	cmd.cmd.opcode = 0x06;
	cmd.prp1 = buffer;
	cmd.cmd.command_id = device->next_command_id++;
	nvme_send_command(device, &cmd, 1);
	return 1;
}
