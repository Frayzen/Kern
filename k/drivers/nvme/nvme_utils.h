#ifndef UTILS_H
#define UTILS_H
#include "drivers/nvme/nvme.h"
#include "assert.h"

#define NVME_ADMIN_QUEUE 1
#define NVME_IO_QUEUE 0
void nvme_send_command(struct submission_q_entry *cmd, u8 is_admin);
void nvme_process_cq(struct nvme_queue *compl_queue);
void nvme_wait_queue(struct nvme_queue *compl_queue);

#endif /* !UTILS_H */
