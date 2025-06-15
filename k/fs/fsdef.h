#ifndef FSDEF_H
#define FSDEF_H

#include "drivers/config.h"
#include "fs/isofs/iso.h"

union fs_data {
	struct fs_iso_data iso;
};

struct filesystem {
	const struct filesystem_impl *impl;
  union fs_data data;
  char mount_path[PATH_LEN];
};

#endif /* !FSDEF_H */
