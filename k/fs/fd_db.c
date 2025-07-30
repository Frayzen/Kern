#include "fd_db.h"
#include "fs/fs.h"
#include "panic.h"

static struct filedesc fds[MAX_FD] = {};
static u8 fds_bitmap[MAX_FD / 8] = {}; // 1 if used, 0 if free

static u8 find_free_id(void)
{
	for (int i = 0; i < MAX_FD / 8; i++) {
		u8 cur = fds_bitmap[i];
		if (cur != 255) {
			int j = 0;
			for (; j < 8; j++) {
				if (!(cur & (1 << j))) {
					break;
				}
			}
			fds_bitmap[i] |= (1 << j);
			return 8 * i + j;
		}
	}
	panic("No more fd available");
	return 255;
}

u8 store_fd(struct filedesc *fd)
{
	int fd_id = find_free_id();
	fd->fd_id = fd_id;
	fds[fd_id] = *fd;
	return fd_id;
}

int free_fd(u8 fd_id)
{
	if (!(fds_bitmap[fd_id / 8] & (1 << (fd_id % 8))))
		return 1;
	struct filedesc zerodesc = {};
	fds[fd_id] = zerodesc;
	fds_bitmap[fd_id / 8] &= ~(1 << (fd_id % 8));
	return 0;
}
struct filedesc *load_fd(int fd_id)
{
	if (!(fds_bitmap[fd_id / 8] & (1 << (fd_id % 8))))
		return NULL;
	return fds + fd_id;
}
