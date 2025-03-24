#ifndef UTILS_H
#define UTILS_H
#include "drivers/nvme/nvme.h"
#include "drivers/pci/pci.h"

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

inline u32 get_bar(struct nvme_device *dev, u16 bar)
{
	return pci_config_read(dev->pci.bus, dev->pci.slot, 0, bar);
}

inline u32 submission_tail(struct nvme_device *device)
{
	// admin queue has identifier 0
	return nvme_read_reg(device,
			     NVME_SQxTDBL(0, device->capability_stride));
}

inline u32 completion_head(struct nvme_device *device)
{
	// admin queue has identifier 0
	return nvme_read_reg(device,
			     NVME_CQxHDBL(0, device->capability_stride));
}

#endif /* !UTILS_H */
