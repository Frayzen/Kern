#ifndef NVME_ADMIN_H
#define NVME_ADMIN_H

#include "drivers/nvme/nvme.h"

int create_io_submission_queue(struct nvme_device *dev);
int create_io_completion_queue(struct nvme_device *dev);
int create_admin_submission_queue(struct nvme_device *dev);
int create_admin_completion_queue(struct nvme_device *dev);
int nvme_identify(struct nvme_device *device);

#endif /* !NVME_ADMIN_H */
