#include "fs.h"
#include "drivers/disk/disk.h"
#include "fs/fd_db.h"
#include "fs/fsdef.h"
#include "fs/isofs/iso.h"
#include "k/atapi.h"
#include "memory.h"
#include <stdio.h>
#include <string.h>

static struct cache *cache = NULL;

#define MAX_FS 10
static struct filesystem fs_list[MAX_FS]; //tmp
static int fs_nb = 0;

#define CUR_BLK(Fd) ((Fd)->offset / CD_BLOCK_SZ)
#define RLTV_OFFSET(Fd) ((Fd)->offset % CD_BLOCK_SZ)

int open(char *path)
{
	for (int i = 0; i < fs_nb; i++) {
		struct filesystem *cur = fs_list + i;
		printf("Check %s and %s of size %d\n", path, cur->mount_path,
		       strlen(cur->mount_path));
		if (!strncmp(path, cur->mount_path, strlen(cur->mount_path))) {
			static struct filedesc fd;
			if (cur->impl->open(cur, path, &fd)) {
				fd.cache = cache_alloc(cache);
				fd.fd_id = store_fd(&fd);
				return fd.fd_id;
			}
		}
	}
	return INVALID_FD;
}

int read(int fd, char *buf, unsigned int len)
{
	struct filedesc *fdptr = load_fd(fd);
	if (fdptr == NULL)
		return -1;
	return fdptr->fs->impl->read(fdptr, buf, len);
}
int seek(int fd, int offset, int whence)
{
	struct filedesc *fdptr = load_fd(fd);
	if (fdptr == NULL)
		return -1;
	return fdptr->fs->impl->seek(fdptr, offset, whence);
}
int close(int fd)
{
	struct filedesc *fdptr = load_fd(fd);
	if (fdptr == NULL)
		return -1;
	int res = fdptr->fs->impl->close(fdptr);
	if (!res)
		return 0;
	cache_free(cache, fdptr->cache);
	return res;
}

const struct filesystem *setup_fs(void)
{
	cache = cache_new((void *)0xF00000, MAX_FD, CD_BLOCK_SZ);
	static struct filesystem cur_fs = {};
	cur_fs.mount_path[0] = '/';
	cur_fs.mount_path[1] = '\0';
	// assert(setup_vfs(fs_list));
	// fs_nb++;
	setup_disk(); // prepare disk read
	if (setup_iso(&cur_fs)) {
		fs_list[fs_nb++] = cur_fs;
		printf("ISOFS is setup !\n");
	}
	return NULL;
}
