#ifndef ISO_H
#define ISO_H

#include "fs/fs.h"
#include <k/types.h>

/**
 * Setup the iso filesystem
 * @param fs the filesytem to update accordingly
 * returns zero if not found, non zero otherwise
 */
int setup_iso(struct filesystem* fs);

extern const struct filesystem_impl fs_iso_impl;

struct fs_iso_data {
  u32 root_blk;
};

#endif /* !ISO_H */
