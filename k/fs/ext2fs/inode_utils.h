#ifndef INODE_UTILS_H
#define INODE_UTILS_H

#include "fs/ext2fs/ext2.h"
#include "fs/fsdef.h"
#include "k/types.h"
int find_inode(const struct filesystem *fs, u32 inode, struct ext2_inode *res);

int find_dir_entry(struct filesystem *fs, const char *token, struct ext2_inode *cur_inode);

#endif /* !INODE_UTILS_H */
