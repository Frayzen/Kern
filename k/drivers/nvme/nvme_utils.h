#ifndef UTILS_H
#define UTILS_H
#include "drivers/nvme/nvme.h"
#include "assert.h"

u32 nvme_read_reg(struct nvme_device *dev, u32 offset)
{
	assert(dev->base_addr != 0x0);
	volatile u32 *nvme_reg = (volatile u32 *)(dev->base_addr + offset);
	return *nvme_reg;
}

void nvme_write_reg(struct nvme_device *dev, u32 offset, u32 value)
{
	assert(dev->base_addr != 0x0);
	volatile u32 *nvme_reg = (volatile u32 *)(dev->base_addr + offset);
	*nvme_reg = value;
}

struct nvme_completion *get_comp_head(struct nvme_device *dev)
{
	return (struct nvme_completion *)(dev->io_cmpl_q.address +
					  sizeof(struct nvme_completion) *
						  dev->c_head);
}

struct nvme_command_entry *get_subm_tail(struct nvme_device *dev)
{
	return (struct nvme_command_entry *)(dev->io_subm_q.address +
	       sizeof(struct nvme_command_entry) * dev->s_tail);
}

#endif /* !UTILS_H */
