#include "nvme_admin.h"
#include "drivers/nvme/nvme.h"
#include "drivers/nvme/nvme_utils.h"
#include "memalloc/memalloc.h"

volatile u32 *nvme_subm_doorbell(u32 queue_id)
{
	return nvme_reg(0x1000 +
			(2 * queue_id * (4 << nvme_dev->capability_stride)));
}
volatile u32 *nvme_cmpl_doorbell(u32 queue_id)
{
	return nvme_reg(0x1000 + ((2 * queue_id + 1) *
				  (4 << nvme_dev->capability_stride)));
}

int create_io_submission_queue()
{
	// Create queue
	nvme_dev->io_subm_q.address = (u64)mmap();
	if (nvme_dev->io_subm_q.address == 0)
		return 0;
	nvme_dev->io_subm_q.size = SUBM_Q_SIZE - 1;
	nvme_dev->io_subm_q.door_bell = nvme_subm_doorbell(1);

	// Create command
	struct submission_q_entry cmd = {};
	cmd.cmd.opcode = OPCODE_IO_SUBMISSION_QUEUE_CREATE;
	// dword 6-7 = base address
	cmd.prp1 = nvme_dev->io_subm_q.address;
	// dword10
	u16 queue_id = 1;
	u32 queue_size = nvme_dev->io_subm_q.size;
	cmd.command_specific[0] = ((u16)queue_size << 16) | queue_id;
	// dword11
	u16 flags = FLAG_CONTIGUOUS_QUEUE;
	u32 completion_id = 1;
	cmd.command_specific[1] = ((u16)completion_id << 16) | flags;

	// Send commmand
	nvme_send_command(&cmd, NVME_ADMIN_QUEUE);
	return 1;
}

int create_io_completion_queue()
{
	// Create queue
	nvme_dev->io_cmpl_q.address = (u64)mmap();
	if (nvme_dev->io_cmpl_q.address == 0)
		return 0;
	nvme_dev->io_cmpl_q.size = COMPL_Q_SIZE - 1;
	nvme_dev->io_cmpl_q.door_bell = nvme_cmpl_doorbell(1);

	struct submission_q_entry cmd = {};
	cmd.cmd.opcode = OPCODE_IO_COMPLETION_QUEUE_CREATE;

	// dword 6-7 = base address
	cmd.prp1 = nvme_dev->io_cmpl_q.address;

	// dword10
	u16 queue_id = 1;
	u32 queue_size = nvme_dev->io_cmpl_q.size;
	cmd.command_specific[0] = (queue_size << 16) | queue_id;

	// dword11
	u32 vector = 1;
	u32 flags = FLAG_CONTIGUOUS_QUEUE | FLAG_ENABLE_INTS;
	cmd.command_specific[1] = (vector << 16) | flags;

	nvme_send_command(&cmd, 1);
	return 1;
}

int create_admin_submission_queue()
{
	// Create queue
	nvme_dev->adm_subm_q.address = (u64)mmap();
	if (nvme_dev->adm_subm_q.address == 0)
		return 0;
	nvme_dev->adm_subm_q.size = SUBM_Q_SIZE - 1;
	nvme_dev->adm_subm_q.door_bell = nvme_subm_doorbell(0);
	// Write to the register
	*nvme_reg(NVME_ASQ) = nvme_dev->adm_subm_q.address;
	NVME_CHECK_STATUS;
	return 1;
}

int create_admin_completion_queue()
{
	// Create queue
	nvme_dev->adm_cmpl_q.address = (u64)mmap();
	if (nvme_dev->adm_cmpl_q.address == 0)
		return 0;
	nvme_dev->adm_cmpl_q.size = COMPL_Q_SIZE - 1;
	nvme_dev->adm_cmpl_q.door_bell = nvme_cmpl_doorbell(0);
	// Write to the register
	*nvme_reg(NVME_ACQ) = nvme_dev->adm_cmpl_q.address;
	return 1;
}

int nvme_identify()
{
	u64 buffer = (u64)mmap();
	struct submission_q_entry cmd = {};
	cmd.cmd.opcode = 0x06;
	cmd.prp1 = buffer;
	cmd.nsid = 1;
	nvme_send_command(&cmd, NVME_ADMIN_QUEUE);
	return 1;
}

void nvme_process_admin_cq()
{
	nvme_process_cq(&nvme_dev->adm_cmpl_q);
}
