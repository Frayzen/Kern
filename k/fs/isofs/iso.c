
#include "iso.h"
#include "drivers/disk/disk.h"
#include "fs/fs.h"
#include "fs/fsdef.h"
#include "k/atapi.h"
#include "k/iso9660.h"
#include "panic.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define VOLUME_BLOCK(i) (16 + i)
#define VOL_BLK_ID "CD001"
#define BOTH_BYTE_VAL8(x) ((x) & 0xFF)
#define BOTH_BYTE_VAL16(x) ((x) & 0xFFFF)
#define BOTH_BYTE_VAL32(x) ((x) & 0xFFFFFFFF)

// Types
#define BOOT_RECORD_TYPE 0
#define PRIMARY_TYPE 1
#define SUPLEMENTARY_TYPE 2
#define TERMINATOR_TYPE -1
#define ISO_BLOCK_SZ 2048

#define RLTV_OFFSET(Fd) ((Fd)->offset % CD_BLOCK_SZ)
#define CUR_BLK(Fd) ((Fd)->offset / CD_BLOCK_SZ)

static char buffer[DISK_BLOCK_SIZE] __attribute__((aligned(4096)));

/*
 * Get the next file in the directory
 * name is the relative path to the file
 * returns a pointer to the directory entry
 */
static struct iso_dir *get_file(int block, char **name)
{
	char *next_name = *name;
	while (*next_name && *next_name != '/')
		next_name++;
	int name_len = next_name - *name;
	disk_read_block(block, 1, buffer);
	char *cur_buffer = buffer;
	struct iso_dir *dir = (struct iso_dir *)cur_buffer;
	while (1) {
		if (dir->dir_size == 0)
			return NULL;
		if (!strncasecmp(dir->idf, *name, name_len)) {
			*name = next_name + 1;
			return dir;
		}
		cur_buffer += dir->dir_size;
		dir = (struct iso_dir *)cur_buffer;
	}
}

static int find(struct filesystem *fs, char *name, u32 *size)
{
	while (name[0] == '/' && *name)
		name++;
	int cur_dir = fs->data.iso.root_blk;
	while (1) {
		struct iso_dir *dir = get_file(cur_dir, &name);
		if (!dir) {
			printf("No such file or directory (%s not found)\n",
			       name);
			return 0;
		}
		cur_dir = dir->data_blk.le;
		if (!(dir->type & ISO_FILE_ISDIR)) {
			*size = dir->file_size.le;
			return dir->data_blk.le;
		}
	}
}

int setup_iso(struct filesystem *fs)
{
	struct iso_prim_voldesc *primary;
	int cur = 0;
	static int root_dir = -1;
	do {
		if (!disk_read_block(VOLUME_BLOCK(cur++), 1, buffer)) {
			printf("Could not read the first bloc, aborting\n",
			       cur);
			return 0;
		}
		if (strncmp(buffer + 1, VOL_BLK_ID, sizeof(VOL_BLK_ID) - 1)) {
			printf("Not an ISO fs\n", cur);
			return 0;
		}
		switch (*buffer) {
		case BOOT_RECORD_TYPE:
			printf("[Block %d] Boot record\n", cur);
			break;
		case PRIMARY_TYPE:
			printf("[Block %d] Primary filesystem\n", cur);
			primary = (struct iso_prim_voldesc *)buffer;
			assert(primary->vol_desc_type == PRIMARY_TYPE);
			assert(primary->vol_blk_size.le == ISO_BLOCK_SZ);
			disk_read_block(primary->root_dir.data_blk.le, 1,
					buffer);
			struct iso_dir *dir = (struct iso_dir *)buffer;
			root_dir = dir->data_blk.le;
			break;
		case SUPLEMENTARY_TYPE:
			printf("[Block %d] Secondary filesystem\n", cur);
			break;
		case TERMINATOR_TYPE:
			printf("[Block %d] Terminator descriptor\n", cur);
			break;
		default:
			panic("Unknown type of descriptor (got %x)", buffer[0]);
		}
	} while (buffer[0] != TERMINATOR_TYPE);
	if (root_dir == -1) {
		return 0;
	}
	fs->impl = &fs_iso_impl;
	fs->data.iso.root_blk = root_dir;
	return 1;
}

int iso_open_handler(struct filesystem *fs, char *path, struct filedesc *fd)
{
	int blk = find(fs, path, &fd->size);
	if (blk == 0)
		return 0;
	strcpy(fd->path, path);
	fd->block = blk;
	fd->fs = fs;
	disk_read_block(fd->block, 1, (char *)fd->cache);
	return 1;
}

int iso_close_handler(struct filedesc *fd)
{
	printf("Closing file %s\n", fd->path);
	return 1;
}

ssize_t iso_read_handler(struct filedesc *fd, char *buf, size_t len)
{
	unsigned int curlen = 0;
	while (curlen != len) {
		if (fd->offset == fd->size)
			return curlen;
		buf[curlen++] = fd->cache[RLTV_OFFSET(fd)];
		fd->offset++;
		if (!RLTV_OFFSET(fd)) // We are at the end of a block
			disk_read_block(fd->block + CUR_BLK(fd) + 1, 1,
					(char *)fd->cache);
	}
	buf[curlen] = 0;
	return curlen;
}

int iso_seek_handler(struct filedesc *fd, int offset, int whence)
{
	u32 prev_blk = CUR_BLK(fd);
	int next_offset;
	switch (whence) {
	case SEEK_SET:
		next_offset = offset;
		break;
	case SEEK_CUR:
		next_offset = fd->offset + offset;
		break;
	case SEEK_END:
		next_offset = fd->size + offset;
		break;
	default:
		return -1;
	}
	if (next_offset < 0 || (u32)next_offset > fd->size)
		return -1;
	fd->offset = next_offset;
	if (CUR_BLK(fd) != prev_blk)
		disk_read_block(fd->block + CUR_BLK(fd) + 1, 1,
				(char *)fd->cache);
	return next_offset;
}

const struct filesystem_impl fs_iso_impl = {
	.open = iso_open_handler,
	.close = iso_close_handler,
	.seek = iso_seek_handler,
	.read = iso_read_handler,
};
