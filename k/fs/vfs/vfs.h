#ifndef VFS_H
#define VFS_H

#include "fs/fsdef.h"
#include <k/types.h>

/**
 * Setup the vfs filesystem
 * returns zero if not found, non zero otherwise
 */
int setup_vfs(struct filesystem* fs);

#endif /* !VFS_H */
