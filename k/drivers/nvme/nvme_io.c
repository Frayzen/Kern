#include "nvme_io.h"
#include "memalloc/memalloc.h"
#include "nvme.h"
#include "k/types.h"
#include <stdio.h>
#include <string.h>

void *get_queue_ptr(struct nvme_queue *q)
{
	return (void *)(q->address +
			sizeof(struct submission_q_entry) * *q->door_bell);
}

void nvme_send_command(struct nvme_device *device,
		       struct submission_q_entry *cmd, u8 is_admin)
{
	struct nvme_queue *sub_queue = is_admin ? &device->adm_subm_q :
						  &device->io_subm_q;
	struct nvme_queue *compl_queue = is_admin ? &device->adm_cmpl_q :
						    &device->io_cmpl_q;

	struct submission_q_entry *subm_tail = get_queue_ptr(sub_queue);
	struct completion_q_entry *compl_head = get_queue_ptr(compl_queue);

	memcpy(subm_tail, cmd, sizeof(struct submission_q_entry));
	(*sub_queue->door_bell)++;
	nvme_wait_status_ready(device);
	if (*sub_queue->door_bell == sub_queue->size)
		*sub_queue->door_bell = 0;
	CHECK_FATAL_STATUS(device);

	printf("Command %d sent correctly\n", cmd->cmd.command_id);
	printf("Checking 0x%x\n", compl_head);
	while (!compl_head->phase_bit)
		;
	printf("Command %d happened correctly\n", cmd->cmd.command_id);

	// Completion aknowledged, reset the bit
	compl_head->phase_bit = 0;
	printf("Got status 0x%x\n", compl_head->status);
	assert(compl_head->status == 0);

	(*compl_queue->door_bell)++;
	if (*compl_queue->door_bell == compl_queue->size)
		*compl_queue->door_bell = 0;
}

int nvme_read(struct nvme_device *dev, u64 lba, u32 sector_count, void *buffer)
{
	struct submission_q_entry cmd = {};

	cmd.cmd.opcode = 0x2;
	cmd.command_specific[0] = (u32)lba;
	cmd.command_specific[1] = (u32)(lba >> 32);
  cmd.command_specific[2] = sector_count;
  cmd.prp1 = (u32) buffer;
  cmd.prp2 = (u32) buffer + PAGE_SIZE;

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
