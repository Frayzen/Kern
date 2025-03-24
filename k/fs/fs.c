#include "fs.h"
#include "drivers/atapi/atapi.h"
#include "fs/isofs/iso_driver.h"
#include "k/atapi.h"
#include "memory.h"
#include "panic.h"
#include "k/kstd.h"

static struct cache *cache = NULL;

struct fd {
	u8 used;
	u32 offset;
	u32 block;
	u32 size; // nb of total bytes
	char *ptr;
};

#define MAX_FDS 1
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
	cache_free(cache, fds[fd].ptr);
	fds[fd].used = 0;
	return 0;
}

void setup_fs(void)
{
	cache = cache_new((void *)0xF00000,
			  MAX_FDS, // Base address of user code
			  CD_BLOCK_SZ);
	while (!setup_atapi())
		;
	if (!setup_iso())
  {
    panic("No root filesystem found");
  }
}
