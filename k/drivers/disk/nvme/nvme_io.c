#include "nvme_io.h"
#include "drivers/disk/nvme/nvme_utils.h"
#include "nvme.h"
#include "k/types.h"

int nvme_read(u64 lba, u32 sector_count, void *buffer)
{
	if (!buffer || sector_count == 0)
		return 0; // Error
	struct submission_q_entry cmd = {};
	u64 buf = (u64)buffer;

	cmd.cmd.opcode = 0x02; // Read opcode
	cmd.nsid = 1;
	cmd.prp1 = buf;
	cmd.command_specific[0] = (u32)lba;
	cmd.command_specific[1] = (u32)(lba >> 32);
	cmd.command_specific[2] = (sector_count - 1) & 0xFFFF; // 0-based value

	// // If buffer crosses page boundary, set PRP2 appropriately
	// if ((buf & ~PAGE_MASK) != ((buf + sector_count * 512 - 1) & ~PAGE_MASK)) {
	//     // Need to handle PRP2 for this case
	//     // This is a simplified example - actual implementation would need proper PRP handling
	// }

	nvme_send_command(&cmd, NVME_IO_QUEUE);
	nvme_sync();
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
