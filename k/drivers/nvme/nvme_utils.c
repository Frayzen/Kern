#include "nvme_utils.h"

static void *get_queue_ptr(struct nvme_queue *q, u8 is_submission)
{
	u32 offset = is_submission ? sizeof(struct submission_q_entry) :
				     sizeof(struct completion_q_entry);
	return (void *)(q->address + offset * q->ptr);
}

void nvme_send_command(struct submission_q_entry *cmd, u8 is_admin)
{
	cmd->cmd.command_id = nvme_dev->next_command_id++;
	struct nvme_queue *sub_queue = is_admin ? (&nvme_dev->adm_subm_q) :
						  &nvme_dev->io_subm_q;
	struct submission_q_entry *subm_tail = get_queue_ptr(sub_queue, 1);
	*subm_tail = *cmd;
	*(sub_queue->door_bell) = ++(sub_queue->ptr);
	nvme_wait_status_ready(nvme_dev);
	if (sub_queue->ptr == sub_queue->size)
		sub_queue->ptr = 0;
	CHECK_FATAL_STATUS(nvme_dev);
	printf("Command 0x%x sent...\n", cmd->cmd.command_id);
}


void nvme_process_cq(struct nvme_queue *compl_queue)
{
	volatile struct completion_q_entry *compl_head =
		get_queue_ptr(compl_queue, 0);
	assert(compl_head->phase_bit);
	CHECK_FATAL_STATUS(nvme_dev);
	if (compl_head->status != 0)
		printf("[ERROR] /!\\ while processing command status : 0x%x\n",
		       compl_head->status);
	else
		printf("Command %d happened correctly\n",
		       compl_head->command_id);
	// Completion aknowledged, reset the bit
	compl_head->phase_bit = 0;
	*compl_queue->door_bell = ++(compl_queue->ptr);
	if (compl_queue->ptr == 0)
		compl_queue->ptr = 0;
	return;
}
