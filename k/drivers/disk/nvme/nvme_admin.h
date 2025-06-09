#ifndef NVME_ADMIN_H
#define NVME_ADMIN_H

int create_io_submission_queue();
int create_io_completion_queue();
int create_admin_submission_queue();
int create_admin_completion_queue();
int nvme_identify();

void nvme_process_admin_cq();

#endif /* !NVME_ADMIN_H */
