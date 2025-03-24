#ifndef UTILS_H
#define UTILS_H
#include "drivers/nvme/nvme.h"

inline u32 nvme_read_reg(struct nvme_device *dev, u32 offset)
{
	volatile u32 *nvme_reg = (volatile u32 *)(dev->base_addr + offset);
	return *nvme_reg;
}

inline void nvme_write_reg(struct nvme_device *dev, u32 offset, u32 value)
{
	volatile u32 *nvme_reg = (volatile u32 *)(dev->base_addr + offset);
	*nvme_reg = value;
}

#endif /* !UTILS_H */
