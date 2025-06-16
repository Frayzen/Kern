#ifndef DISK_H
#define DISK_H

#include "k/types.h"

#define DISK_BLOCK_SIZE 2048

void setup_disk();
int disk_read_block(u64 block, u64 nb_block, void *buffer);

#endif /* !DISK_H */
