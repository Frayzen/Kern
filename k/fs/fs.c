#include "fs.h"
#include "k/drivers/atapi.h"
#include "k/kstd.h"
#include "memory.h"
#include "panic.h"

struct fd {
	u8 used;
	u32 offset;
	u32 block;
	u32 size; // nb of total bytes
	char *ptr;
};

#define MAX_FDS 255
static struct fd fds[MAX_FDS];

#define CUR_BLK(Fd) ((Fd)->offset / CD_BLOCK_SZ)
#define RLTV_OFFSET(Fd) ((Fd)->offset % CD_BLOCK_SZ)

int open(char *path)
{
	int id;
	for (id = 0; id < MAX_FDS; id++) {
		if (!fds[id].used)
			break;
	}
	if (id == MAX_FDS)
		panic("Too many files open");
	struct fd *fd = &fds[id];
	fd->block = find(path, &fd->size);
	if (fd->block == 0)
		return -1;
	fd->ptr = cache_alloc(cache);
	read_block(fd->block, 1, fd->ptr);
	fd->used = 1;
	return id;
}
int read(int fd, char *buf, unsigned int len)
{
	if (!fds[fd].used)
		return -1;
	struct fd *p = &fds[fd];
	unsigned int curlen = 0;
	while (curlen != len) {
		if (p->offset == p->size)
			return curlen;
		buf[curlen++] = p->ptr[RLTV_OFFSET(p)];
		p->offset++;
		if (!RLTV_OFFSET(p)) // We are at the end of a block
			read_block(p->block + CUR_BLK(p) + 1, 1, p->ptr);
	}
	buf[curlen] = 0;
	return curlen;
}
int seek(int fd, int offset, int whence)
{
	if (!fds[fd].used)
		return -1;
	int next_offset;
	struct fd *f = fds + fd;
	switch (whence) {
	case SEEK_SET:
		next_offset = offset;
		break;
	case SEEK_CUR:
		next_offset = f->offset + offset;
		break;
	case SEEK_END:
		next_offset = f->size + offset;
		break;
	default:
		return -1;
	}
	if (next_offset < 0 || (u32)next_offset > f->size)
		return -1;
	f->offset = next_offset;
	return next_offset;
}
int close(int fd)
{
	if (!fds[fd].used)
		return -1;
	fds[fd].used = 0;
	cache_free(cache, fds[fd].ptr);
	return 0;
}

static struct cache *cache;
void setup_fs(void)
{
	cache = cache_new((void *)0xF000000,
			  0xF000, // Base address of user code
			  CD_BLOCK_SZ);
	setup_fs();
	while (!setup_atapi())
		;
	struct iso_prim_voldesc *primary;
	int cur = 0;
	do {
		if (!read_block(VOLUME_BLOCK(cur++), 1, buffer)) {
			printf("An error occured, retrying...\n", cur);
			setup_iso();
		}
		if (strncmp(buffer + 1, VOL_BLK_ID, sizeof(VOL_BLK_ID) - 1))
			panic("Invalid filesystem");
		switch (*buffer) {
		case BOOT_RECORD_TYPE:
			printf("[Block %d] Boot record\n", cur);
			break;
		case PRIMARY_TYPE:
			printf("[Block %d] Primary filesystem\n", cur);
			primary = (struct iso_prim_voldesc *)buffer;
			assert(primary->vol_desc_type == PRIMARY_TYPE);
			assert(primary->vol_blk_size.le == CD_BLOCK_SZ);
			read_block(primary->root_dir.data_blk.le, 1, buffer);
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
	if (root_dir == -1)
		panic("No root directory found");
	println("Filesystem setup");
}
