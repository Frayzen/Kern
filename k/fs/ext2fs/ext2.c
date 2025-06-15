#include "ext2.h"
#include "fs/fs.h"
#include "fs/fsdef.h"

#define SUPERBLOCK_LOC 1024 // in byte

int setup_ext2()
{
}

int ext2_open_handler(struct filesystem *fs, char *path, struct filedesc *fd)
{
}
int ext2_close_handler(struct filedesc *fd)
{
}
ssize_t ext2_read_handler(struct filedesc *fd, char *buf, size_t len)
{
}
int ext2_seek_handler(struct filedesc *fd, int offset, int whence)
{
}

const struct filesystem_impl fs_ext2_impl = {
	.open = ext2_open_handler,
	.close = ext2_close_handler,
	.seek = ext2_seek_handler,
	.read = ext2_read_handler,
};
