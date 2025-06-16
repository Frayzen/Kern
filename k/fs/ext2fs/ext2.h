#ifndef EXT_2_H
#define EXT_2_H

#include "fs/fs.h"
#include "k/kfs.h"
#include <k/types.h>

extern const struct filesystem_impl fs_ext2_impl;
extern char ext2_buffer[];

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

enum ext2_type_perm : u8 {
	TP_FIFO = 0x1,
	TP_CHAR_DEV = 0x2,
	TP_DIR = 0x4,
	TP_BLK_DEV = 0x6,
	TP_FILE = 0x8,
	TP_SYM_LINK = 0xA,
	TP_UNIX_SOCKET = 0xC,
};

enum ext2_flags : u32 {
	// the followings are not used
	FLG_SECURE_DEL = 0x00000001,
	FLG_KEEP_DATA_OND_DEL = 0x00000002,
	FLG_FILE_COMPRESSION = 0x00000004,
	// those ones are
	FLG_SYNC_UPDATE =
		0x00000008, // Synchronous updates—new data is written immediately to disk
	FLG_IMMUTABLE =
		0x00000010, // Immutable file (content cannot be changed)
	FLG_APPEND_ONLY = 0x00000020,
	FLG_NO_DUMP = 0x00000040, //	File is not included in 'dump' command
	FLG_NO_ACCESS_TIME_UPDATE =
		0x00000080, //	Last accessed time should not updated
	FLG_HASH_INDEX_DIR = 0x00010000, //	Hash indexed directory
	FLG_AFS_DIR = 0x00020000, //	AFS directory
	FLG_JOURNAL_FILE = 0x00040000, //	Journal file data
};

#define EXT2_INODE_NB_DIRECT_BLK 12

struct ext2_inode {
	u16 perm : 12;
	enum ext2_type_perm type_perm : 4;
	u16 user_id;
	u32 low_size;
	u32 last_access_time; // posix time
	u32 creation_time; // posix time
	u32 last_modif_time; // posix time
	u32 deletion_time; // posix time
	u16 group_id;
	u16 hard_link_count;
	u32 disk_sector_count;
	enum ext2_flags flags;
	u32 os_specific;
	u32 direct_block_ptr[EXT2_INODE_NB_DIRECT_BLK];
	u32 indirect_block_ptr;
	u32 doubly_indirect_block_ptr;
	u32 triply_indirect_block_ptr;
	u32 generation_nb; // Primarily used for NFS (File ACL)
	// For a file : upper_size
	// For a dir : Directory ACL
	u32 upper_size; // only if ext2 major version != 0
	u32 fragment_block_addr;
	u32 os_specific2[3];
} __packed;

struct ext2_blk_grp_desc {
	u32 blk_usage_bitmap_blkaddr;
	u32 inode_usage_bitmap_blkaddr;
	u32 inode_table_blk;
	u16 nb_unallocated_blk;
	u16 nb_unallocated_inode;
	u16 nb_dirs;
	u8 __unused[14];
} __packed;

enum ext2_dir_entry_type : u8 {
	DET_UNKOWN = 0,
	DET_REG_FILE = 1,
	DET_DIR = 2,
	DET_CHARACTER_DEV = 3,
	DET_BLK_DEV = 4,
	DET_FIFO = 5,
	DET_SOCKET = 6,
	DET_SYMLINK = 7,
};

struct ext2_dir_entry {
	u32 inode;
	u16 size;
	u8 name_len;
	enum ext2_dir_entry_type type;
	char name[];
};

struct fs_ext2_data {
	struct ext2_base_superblock superblock;
	u32 nb_block_grp;
	u32 blk_size;
};

/**
 * Setup the ext2 filesystem
 * returns zero if not found, non zero otherwise
 */
int setup_ext2(struct filesystem *fs);

#endif /* !EXT_2_H */
