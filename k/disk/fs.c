#include "fs.h"
#include "assert.h"
#include "disk/atapi.h"
#include "k/atapi.h"
#include "k/types.h"
#include "panic.h"
#include "serial.h"
#include "stdio.h"
#include "string.h"

#define VOLUME_BLOCK(i) (16 + i)
#define VOL_BLK_ID "CD001"
#define BOTH_BYTE_VAL8(x) ((x) & 0xFF)
#define BOTH_BYTE_VAL16(x) ((x) & 0xFFFF)
#define BOTH_BYTE_VAL32(x) ((x) & 0xFFFFFFFF)

// Types
#define BOOT_RECORD_TYPE 0
#define PRIMARY_TYPE 1
#define SUPLEMENTARY_TYPE 2
#define TERMINATOR_TYPE -1

struct directory_entry {
	u8 length; // 1
	u8 ext_attr; // 2
	u8 loc_ext_attr[8]; // 3 to 10
	u8 data_length[8]; // 11 to 18
	u8 time[7]; // 19 to 25
	u8 file_flag; // 26
	u8 file_unit_size; // 27
	u8 interleave_gap_size; // 28
	u8 vol_seq_num[4]; // 29 to 32
	u8 len_id; // 33
	char id[]; // 34 to end (file name)
} __packed;

struct primary_volume_desc {
	u8 type; // 1
	u8 std_id[5]; // 2 to 6
	u8 __unused0[34]; // 7 to 40
	u8 vol_id[32]; // 41 to 72
	u8 __unused1[8]; // 73 to 80
	u64 vol_space_size; // 81 to 88
	u8 __unused2[40]; // 89 to 128
	u32 log_blk_size; // 129 to 132
	u64 path_table_bytes; // 133 to 140
	u32 l_path_table; // 141 to 144
	u8 __unused3[4]; // 145 to 148
	u32 m_path_table; // 149 to 152
	u8 __unused4[4]; // 153 to 156
	struct directory_entry root; //157 to 189
} __packed;

struct path_table_entry {
	u8 len_id; // 1
	u8 len_ext_attr; // 2
	u8 loc_ext_attr[4]; // 3 to 6
	u16 parent_dir_nb; // 7 to 8
	char id[];
} __packed;

int strncmp(const char *s1, const char *s2, size_t n)
{
	for (size_t i = 0; i < n; i++)
		if (s1[i] != s2[i])
			return 1;
	return 0;
}

static char buffer[CD_BLOCK_SZ];
static struct primary_volume_desc primary;
void setup_filesystem(void)
{
	char *primary_buffer = (char *)&primary;
	while (!setup_atapi())
		;
	int cur = 0;
	printf("SIZE : %d\n", sizeof(struct primary_volume_desc));
	printf("SIZE DIR : %d\n", sizeof(struct directory_entry));
	do {
		if (!read_block(VOLUME_BLOCK(cur++), 1, buffer)) {
			printf("An error occured, retrying...\n", cur);
			setup_filesystem();
		}
		if (strncmp(buffer + 1, VOL_BLK_ID, sizeof(VOL_BLK_ID) - 1))
			panic("Invalid filesystem");
		switch (*buffer) {
		case BOOT_RECORD_TYPE:
			printf("[Block %d] Boot record\n", cur);
			break;
		case PRIMARY_TYPE:
			printf("[Block %d] Primary filesystem\n", cur);
			memcpy(primary_buffer, buffer, CD_BLOCK_SZ);
			break;
		case SUPLEMENTARY_TYPE:
			printf("[Block %d] Secondary filesystem\n", cur);
			break;
		case TERMINATOR_TYPE:
			printf("[Block %d] Terminator descriptor\n", cur);
			break;
		default:
			panic("Unknown type of descriptor (got %x)", buffer[0]);
		}
	} while (buffer[0] != TERMINATOR_TYPE);

	printf("Primary volume descriptor:\n");
	printf("Std ID: %s\n", primary.std_id);
	printf("Vol ID: %s\n", primary.vol_id);
	printf("Log block size: %d\n", BOTH_BYTE_VAL16(primary.log_blk_size));
	printf("LBLKSIZE: ");
	for (int i = 0; i < 32 / 4; i++)
		printf("%x", (primary.log_blk_size >> i * 4) & 0xF);
	printf("\n");

	assert(primary.type == PRIMARY_TYPE);
	assert(BOTH_BYTE_VAL16(primary.log_blk_size) == CD_BLOCK_SZ);

	u32 path_table_size = BOTH_BYTE_VAL32(primary.path_table_bytes);
	u32 path_table = primary.l_path_table;
	printf("LPath table: 0x%x\n", path_table);
	printf("Path table size: 0x%x\n", path_table_size);
	printf("Path table:\n");

	read_block(path_table, 1, buffer);

	char *path_entry = buffer;
	while (path_entry < buffer + path_table_size) {
		struct path_table_entry *entry =
			(struct path_table_entry *)path_entry;
		printf("Size: %d\n", entry->len_id);
		for (int j = 0; j < entry->len_id; j++)
			printf("%c", entry->id[j]);
		printf("\n");
		path_entry += sizeof(struct path_table_entry);
		path_entry += entry->len_id;
        if (entry->len_id % 2 == 1)
            path_entry++;
		read();
	}

	println("Filesystem setup");
}
