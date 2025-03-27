#include "nvme_io.h"
#include "nvme.h"
#include "k/types.h"
#include <stdio.h>
#include <string.h>

void *get_queue_ptr(struct nvme_queue *q)
{
	return (void *)(q->address +
			sizeof(struct submission_q_entry) * *q->door_bell);
}

struct submission_q_entry create_io_command(struct nvme_device *dev, u8 opcode,
					    u8 nsid, void *data, u64 lba,
					    u16 num_blocks)
{
	struct submission_q_entry command_entry = {};
	command_entry.cmd.opcode = opcode;
	command_entry.nsid = nsid;
	command_entry.prp1 = (u32)data;
	command_entry.prp2 = 0;
	command_entry.cmd.command_id = dev->next_command_id++;
	command_entry.command_specific[0] = (u32)lba;
	command_entry.command_specific[1] = (u32)((u64)lba >> 32);
	command_entry.command_specific[2] = (u16)(num_blocks - 1);
	return command_entry;
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
