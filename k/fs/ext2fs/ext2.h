#ifndef EXT_2_H
#define EXT_2_H

#include "fs/fs.h"
#include "k/kfs.h"
#include <k/types.h>

extern const struct filesystem_impl fs_ext2_impl;

enum ext2_fs_state : u16 { EXT2_FS_STATE_CLEAN = 1, EXT2_FS_STATE_ERR = 2 };
enum ext2_err_handler : u16 {
	EXT2_ERR_HDL_CONTINUE = 1,
	EXT2_ERR_HDL_READ_ONLY = 2,
	EXT2_ERR_HDL_PANIC = 3
};

struct ext2_base_superblock {
	u32 total_nb_inode;
	u32 total_nb_block;
	u32 superuser_nb_block;
	u32 total_unallocated_block;
	u32 total_unallocated_inode;
	u32 superblock_id; // Block number of the block containing the superblock
	u32 log_block_size; // the number to shift 1,024 to the left by to obtain the block size
	u32 log_fragment_size; // the number to shift 1,024 to the left by to obtain the fragment size
	u32 block_per_blockgrp;
	u32 fragment_per_blockgrp;
	u32 inode_per_blockgrp;
	u32 last_mount_time; // posix time
	u32 last_written_time; // posix time
	u16 nb_mount_since_fsck;
	u16 nb_mount_before_fsck;
	u16 signature; // 0xef53
	enum ext2_fs_state state;
	enum ext2_err_handler error_handle_method;
	u16 minor_version;
	u32 last_fsck; // posix time
	u32 interval_fsck; // posix time
	u32 os_id; // Linux is 0
	u32 major_version;
	u16 superuser_id;
	u16 supergroup_id;
	struct ext2_extended_superblock { // usable if major_version >= 1
		u32 first_free_inode;
		u16 inode_structure_size;
		u16 current_block_group; // Block group that this superblock is part of (if backup copy)
		u32 optional_features;
		u32 required_features;
		u32 write_required_features; // read only if not supported
		u8 file_system_id[16];
		char volume_name[16]; // null terminated
		char last_mount_path[64]; // null terminated
		u32 compression_algo;
		u8 file_nb_block_preallocate;
		u8 dir_nb_block_preallocate;
		u16 __unused;
		u8 journal_id[16];
		u32 journal_inode;
		u32 journal_device;
		u32 orphan_inode_list_head;
	} extended;
} __packed;

struct fs_ext2_data {
	struct ext2_base_superblock superblock;
	u32 block_size;
};

/**
 * Setup the ext2 filesystem
 * returns zero if not found, non zero otherwise
 */
int setup_ext2(struct filesystem *fs);

#endif /* !EXT_2_H */
