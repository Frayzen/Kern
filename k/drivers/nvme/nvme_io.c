#include "nvme_io.h"
#include "drivers/nvme/nvme_utils.h"
#include "memalloc/memalloc.h"
#include "nvme.h"
#include "k/types.h"
#include <stdio.h>

int nvme_read(u64 lba, u32 sector_count, void *buffer)
{
	struct submission_q_entry cmd = {};

	u64 buf = (u64)buffer;
	cmd.cmd.opcode = 0x02;
	cmd.nsid = 1;
	cmd.command_specific[0] = (u32)lba;
	cmd.command_specific[1] = (u32)(lba >> 32);
	cmd.command_specific[2] = sector_count & 0xFFFF;
	cmd.prp1 = buf;

	nvme_send_command(&cmd, NVME_IO_QUEUE);
	return 1;
}

int nvme_write(u64 lba, u32 sector_count, void *buffer)
{
	return 1;
}

void nvme_process_io_cq()
{
  nvme_process_cq(&nvme_dev->io_cmpl_q);
}
