#ifndef NVME_IO_H
#define NVME_IO_H

// 64 bytes, arranged in 16 DWORDs (1 DWORDS is 4 bytes)
#include "drivers/nvme/nvme.h"
#include "k/types.h"

struct nvme_device_t;
struct nvme_queue_t;

int nvme_write(u64 lba, u32 sector_count, void *buffer);
int nvme_read(u64 lba, u32 sector_count, void *buffer);

void nvme_process_io_cq();

#endif /* !NVME_IO_H */
