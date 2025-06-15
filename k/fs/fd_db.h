#ifndef FD_DB_H
#define FD_DB_H

#include "fs/fs.h"
// Need to be aligned on 8
#define MAX_FD 0x100 
#define INVALID_FD -1

/**
 * return fd id
 */
u8 store_fd(struct filedesc *fd);
/**
 * Return NULL if not valid
 */
struct filedesc *load_fd(int fd);

/**
 * Return 0 on success
 */
int free_fd(u8 fd_id);

#endif /* !FD_DB_H */
