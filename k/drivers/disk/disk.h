#ifndef DISK_H
#define DISK_H

void setup_disk();
int disk_read_block(unsigned int block, unsigned int nb_block, char *buffer);

#endif /* !DISK_H */
