#ifndef NVME_IO_H
#define NVME_IO_H

// 64 bytes, arranged in 16 DWORDs (1 DWORDS is 4 bytes)
#include "k/types.h"

struct nvme_device;
struct nvme_queue;

struct submission_q_entry create_io_command(struct nvme_device *dev, u8 opcode,
					    u8 nsid, void *data, u64 lba,
					    u16 num_blocks);
void nvme_send_command(struct nvme_device *device,
		       struct submission_q_entry *cmd, u8 is_admin);

int nvme_write(struct nvme_device *device, u64 lba, u32 sector_count,
	       void *buffer);
int nvme_read(struct nvme_device *device, u64 lba, u32 sector_count,
	      void *buffer);

#endif /* !NVME_IO_H */
