#include "vfs.h"
#include "fs/fs.h"
#include "fs/fsdef.h"

#define SUPERBLOCK_LOC 1024 // in byte

int setup_vfs(struct filesystem* fs)
{
  return 1; 
}

// struct filedesc vfs_open_handler(char *path)
// {
// }
// int vfs_close_handler(struct filedesc fd)
// {
// }
// ssize_t vfs_read_handler(struct filedesc fd, char *buf, size_t len)
// {
// }
// int vfs_seek_handler(struct filedesc fd, int offset, int whence)
// {
// }

// const struct filesystem_impl fs_vfs_impl = {
// 	.open = vfs_open_handler,
// 	.close = vfs_close_handler,
// 	.seek = vfs_seek_handler,
// 	.read = vfs_read_handler,
// };
