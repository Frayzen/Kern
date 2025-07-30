#ifndef UTILS_H
#define UTILS_H

#include "fs/fsdef.h"

#define ROUND_UP(Value, Diviser) \
	((Value) / (Diviser) + ((Value) % (Diviser) ? 1 : 0))

void *load_block(const struct filesystem *fs, u64 blk_id);
void *load_block_to(const struct filesystem *fs, u64 blk_id, void *buffer);
char *next_path_name(char **path_ptr);

#endif /* !UTILS_H */
