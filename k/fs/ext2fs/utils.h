#ifndef UTILS_H
#define UTILS_H

#include "fs/fsdef.h"

#define ROUND_UP(Value, Diviser) \
	((Value) / (Diviser) + ((Value) % (Diviser) ? 1 : 0))

void *load_block(struct filesystem *fs, u64 blk_id);
char *next_path_name(char **path_ptr);

#endif /* !UTILS_H */
