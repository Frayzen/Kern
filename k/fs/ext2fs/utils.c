#include "utils.h"
#include "drivers/disk/disk.h"
#include <stdio.h>
#include <string.h>

void *load_block(struct filesystem *fs, u64 blk_id)
{
	printf("BLK SIZE IS %d\n", fs->data.ext2.blk_size);
	u64 addr = blk_id * fs->data.ext2.blk_size;
	int nb_blk = ROUND_UP(fs->data.ext2.blk_size, DISK_BLOCK_SIZE);
	u64 from = addr / DISK_BLOCK_SIZE;
	u64 offset = addr % DISK_BLOCK_SIZE;
	printf("LOADING %d BLK FROM BLK %x (offset %d) \n", nb_blk, from, offset);
	disk_read_block(from, nb_blk, ext2_buffer);
	return ext2_buffer + offset;
}

char *next_path_name(char **path_ptr)
{
	static char token[255];
	char *path = *path_ptr;
	while (*path == '/')
		path++;
	int i = 0;
	while (*path != '/' && *path)
		token[i++] = *((path)++);
	if (!*path)
		return NULL;
	token[i] = '\0';
	*path_ptr = path;
	return token;
}
