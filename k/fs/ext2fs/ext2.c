#include "ext2.h"
#include "drivers/disk/disk.h"
#include "fs/fs.h"
#include "fs/fsdef.h"
#include "panic.h"
#include <stdio.h>

#define SUPERBLOCK_LOC 1024 // in byte

static char buffer[BLOCK_SIZE] __attribute__((aligned(4096)));

#define EXT2_SIG 0xef53
int setup_ext2(struct filesystem *fs)
{
  printf("Buffer is %x\n", buffer);
	if (!disk_read_block(0, 1, buffer))
		panic("Could not read disk");
	struct ext2_base_superblock *superblock =
		(struct ext2_base_superblock *)(buffer + SUPERBLOCK_LOC);
	if (superblock->signature != EXT2_SIG) {
		printf("Not an EXT2\n");
		return 0;
	}
	printf("Found EXT2 !\n");
	printf("Block size : %d\n", superblock->log_block_size * 1024);
	fs->impl = &fs_ext2_impl;
	fs->data.ext2.superblock = *superblock;
  fs->data.ext2.block_size = superblock->log_block_size * 1024;
	return 1;
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
