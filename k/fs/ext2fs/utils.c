#include "utils.h"
#include "drivers/disk/disk.h"
#include <stdio.h>
#include <string.h>

void *load_block_to(const struct filesystem *fs, u64 blk_id, void *buffer)
{
	u64 addr = blk_id * fs->data.ext2.blk_size;
	int nb_blk = ROUND_UP(fs->data.ext2.blk_size, DISK_BLOCK_SIZE);
	u64 from = addr / DISK_BLOCK_SIZE;
	u64 offset = addr % DISK_BLOCK_SIZE;
	disk_read_block(from, nb_blk, buffer);
	return buffer + offset;
}
void *load_block(const struct filesystem *fs, u64 blk_id)
{
	return load_block_to(fs, blk_id, ext2_buffer);
}

char *next_path_name(char **path_ptr)
{
	static char token[255];
	char *path = *path_ptr;
	if (!*path)
		return NULL;
	while (*path == '/')
		path++;
	int i = 0;
	while (*path != '/' && *path)
		token[i++] = *((path)++);
	token[i] = '\0';
	*path_ptr = path;
	return token;
}
