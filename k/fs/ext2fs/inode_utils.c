#include "inode_utils.h"
#include "drivers/disk/disk.h"
#include "fs/ext2fs/ext2.h"
#include "fs/ext2fs/utils.h"
#include "fs/fsdef.h"
#include "k/types.h"
#include "panic.h"
#include <stdio.h>
#include <string.h>

int find_inode(struct filesystem *fs, u32 inode,
	       struct ext2_inode *res) // inode starts at 1
{
	struct ext2_base_superblock *sb = &fs->data.ext2.superblock;
	if (inode < 2 || inode > sb->total_nb_inode)
		return 0;

	// Compute Index and Offsets
	u32 index = (inode - 1) % sb->inode_per_blockgrp;
	u32 inode_size =
		sb->major_version > 1 ? 128 : sb->extended.inode_structure_size;
	// The block group id
	u32 block_grp = (inode - 1) / sb->inode_per_blockgrp;
	// The block offset from block group start
	u32 blk_offset = (index * inode_size) / fs->data.ext2.blk_size;
	// The offset from within the block
	u32 byte_offset = (index * inode_size) % fs->data.ext2.blk_size;

	// Load the correct block group descriptor
	// Amount of block group descriptor we can fit in a block (for loading right block of the blk_grp_desc_table)
	const u32 bdesc_per_blk =
		(fs->data.ext2.blk_size / sizeof(struct ext2_blk_grp_desc));
	const u32 start_grp_desc_table = fs->data.ext2.blk_size == 1024 ? 2 : 1;
	struct ext2_blk_grp_desc *blk_grp_desc_table = load_block(
		fs, start_grp_desc_table + block_grp / bdesc_per_blk);
	struct ext2_blk_grp_desc *cur_grp =
		&blk_grp_desc_table[block_grp % bdesc_per_blk];

	// Get the Inode
	void *inode_blk = load_block(fs, cur_grp->inode_table_blk + blk_offset);
	struct ext2_inode *inode_ptr = inode_blk + byte_offset;
	*res = *inode_ptr;
	return 1;
}

int find_dir_entry(struct filesystem *fs, const char *token,
		   struct ext2_inode *cur_inode)
{
	// For all of the dir entry blocks
	// TODO handle undirect access, this is direct only
	for (int i = 0; i < EXT2_INODE_NB_DIRECT_BLK; i++) {
		u32 cur_blk = cur_inode->direct_block_ptr[i];
		if (cur_blk == 0)
			return 0;
		load_block(fs, cur_blk);

		// For all of the dir entries of the current block
		u32 entry_offset = 0;
		struct ext2_dir_entry *cur =
			(void *)(ext2_buffer + entry_offset);
		while (entry_offset < fs->data.ext2.blk_size) {
      
      for (int j = 0; cur->name[j] && token[j]; j++)
      {
        printf("CMP %x and %x\n", cur->name[j], token[j]);
      }
			if (!strncmp(token, cur->name, cur->name_len))
				return cur->inode;
			entry_offset += cur->size;
			cur = (void *)(ext2_buffer + entry_offset);
		}
	}
	return 0;
}
