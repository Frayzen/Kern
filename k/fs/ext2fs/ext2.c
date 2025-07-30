#include "ext2.h"
#include "drivers/disk/disk.h"
#include "fs/ext2fs/inode_utils.h"
#include "fs/ext2fs/utils.h"
#include "fs/fs.h"
#include "fs/fsdef.h"
#include "panic.h"
#include "string.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define SUPERBLOCK_LOC 1024 // in byte
char ext2_buffer[DISK_BLOCK_SIZE] __attribute__((aligned(4096)));

#define EXT2_SIG 0xef53
int setup_ext2(struct filesystem *fs)
{
	printf("Buffer is %x\n", ext2_buffer);
	if (!disk_read_block(0, 1, ext2_buffer))
		panic("Could not read disk");
	struct ext2_base_superblock *superblock =
		(struct ext2_base_superblock *)(ext2_buffer + SUPERBLOCK_LOC);
	if (superblock->signature != EXT2_SIG) {
		printf("Not an EXT2\n");
		return 0;
	}
	printf("Found EXT2 !\n");
	printf("Block size : %d\n", 1024 << superblock->log_block_size);
	fs->impl = &fs_ext2_impl;
	fs->data.ext2.superblock = *superblock;
	fs->data.ext2.nb_block_grp = ROUND_UP(superblock->total_nb_block,
					      superblock->block_per_blockgrp);
	struct ext2_base_superblock *sb = &fs->data.ext2.superblock;
	fs->data.ext2.blk_size = 1024 << sb->log_block_size;
	return 1;
}

#define ROOT_INODE 2

int ext2_open_handler(struct filesystem *fs, char *path, struct filedesc *fd)
{
	u32 cur_inode_id = ROOT_INODE;
	char *token = next_path_name(&path);
	static struct ext2_inode cur_inode;
	char found = 0;
	while (!found) {
		// printf("Looking for TOKEN %s in inode %d\n", token, cur_inode_id);
		if (!find_inode(fs, cur_inode_id, &cur_inode))
			panic("Could not find inode for %s\n", token);
		if (cur_inode.type_perm != TP_DIR) {
			printf("%s is not a folder\n", token);
			return 0;
		};
		if (!(cur_inode_id = find_dir_entry(fs, token, &cur_inode))) {
			printf("Could not find token %s in folder\n", token);
			return 0;
		}
		if (!(token = next_path_name(&path)))
			found = 1;
	}
	// Got the right inode
	printf("Inode is %d\n", cur_inode_id);
	fd->block = cur_inode_id;
	fd->fs = fs;
	return 1;
}

int ext2_close_handler(struct filedesc *fd)
{
	printf("Closing file %s\n", fd->path);
	return 1;
}
ssize_t ext2_read_handler(struct filedesc *fd, char *buf, size_t len)
{
	const struct filesystem *fs = fd->fs;
	struct ext2_inode inode;
	printf("OK\n");
	if (!find_inode(fs, fd->block, &inode)) {
		printf("OK1\n");
		printf("Error: reading from inode %d\n", fd->block);
		return -1;
	}
  printf("OK2\n");
	u64 size = inode.low_size + ((u64)inode.upper_size << 32);
	printf("Size is %d\n", size);
	if (fd->offset + len > size)
		len = size - fd->offset;
	size_t remain = len;
	printf("Remain is %d\n", remain);
	while (remain > 0) {
		size_t blk_nb = fd->offset / DISK_BLOCK_SIZE;
		size_t blk_offset = fd->offset % DISK_BLOCK_SIZE;
		if (!(inode.direct_block_ptr[blk_nb]))
			panic("Inconsistent block %d for fd %s\n", blk_nb,
			      fd->path);
		printf("Block is nb %d\n", inode.direct_block_ptr[blk_nb]);
		uint8_t *blk_buf =
			load_block(fs, inode.direct_block_ptr[blk_nb]);
		size_t cur_size =
			DISK_BLOCK_SIZE - blk_offset; // read till end of BLOCK
		if (cur_size > remain) // do not overread
			cur_size = remain;
		memcpy(buf, blk_buf + blk_offset, cur_size);
		remain -= cur_size;
		fd->offset += cur_size;
	}
	return len;
}
int ext2_seek_handler(struct filedesc *fd, int offset, int whence)
{
	(void)fd;
	(void)offset;
	(void)whence;
	return -1;
}

const struct filesystem_impl fs_ext2_impl = {
	.open = ext2_open_handler,
	.close = ext2_close_handler,
	.seek = ext2_seek_handler,
	.read = ext2_read_handler,
};
