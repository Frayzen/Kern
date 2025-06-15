#ifndef FS_H
#define FS_H

#include "drivers/config.h"
#include "k/kstd.h"
#include "k/types.h"
#include <stddef.h>

struct filedesc {
	const struct filesystem *fs;
	u8 fd_id; // id of the fd associated with database 
	char path[PATH_LEN]; // path of the file
	u32 block; // only relevant if on disk
	u32 size; // size of the file
  u32 offset; // current offset (associated with seek)
  u8* cache; // cache data from a read 
};

typedef int (*fs_open_handler)(struct filesystem* fs, char *path, struct filedesc* fd);
typedef int (*fs_close_handler)(struct filedesc* fd);
typedef ssize_t (*fs_read_handler)(struct filedesc* fd, char *buf, size_t len);
typedef int (*fs_seek_handler)(struct filedesc* fd, int offset, int whence);

struct filesystem_impl {
	fs_open_handler open;
	fs_close_handler close;
	fs_read_handler read;
	fs_seek_handler seek;
};

int close(int fd);
int seek(int fd, int offset, int whence);
int read(int fd, char *buf, unsigned int len);
int open(char *path);

/**
 * Setup the filesystem
 */
const struct filesystem *setup_fs(void);

#endif /* !FS_H */
