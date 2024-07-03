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

struct record_attr {
    u8 __unused0[78]; //1 to 78
    u8 record_format; // 79
    u8 __unused1; // 80
    u32 record_length; // 81 to 84
    u8 __unused2[166]; // 85 to 250
};

struct directory_entry {
	u8 length; // 1
	u8 ext_attr; // 2
	u64 loc_ext_attr; // 3 to 10
	u64 data_length; // 11 to 18
	u8 time[7]; // 19 to 25
	u8 file_flag; // 26
	u8 file_unit_size; // 27
	u8 interleave_gap_size; // 28
	u32 vol_seq_num; // 29 to 32
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
	u32 loc_ext_attr; // 3 to 6
	u16 parent_dir_nb; // 7 to 8
	char id[];
} __packed;

static char buffer[CD_BLOCK_SZ];
static struct primary_volume_desc primary;
static char path_table[CD_BLOCK_SZ];
static u32 path_table_size;

int strncmp(const char *s1, const char *s2, size_t n)
{
	for (size_t i = 0; i < n; i++)
		if (s1[i] != s2[i])
			return 1;
	return 0;
}

void read_path_table()
{
	char *path_entry = buffer;
	while (path_entry < buffer + path_table_size) {
		struct path_table_entry *entry =
			(struct path_table_entry *)path_entry;
		for (int j = 0; j < entry->len_id; j++)
			printf("%c", entry->id[j]);
		printf(" POSITION: %x LENGTH: %x PARENT: %d\n",
		       entry->loc_ext_attr, entry->len_ext_attr,
		       entry->parent_dir_nb);
		path_entry += sizeof(struct path_table_entry);
		path_entry += entry->len_id + (entry->len_id % 2);
	}
}

void print_children(int table_id)
{
	char *path_entry = buffer;
	struct path_table_entry *entry;
	int cur = 0;
	while (path_entry < buffer + path_table_size) {
		entry = (struct path_table_entry *)path_entry;
		path_entry += sizeof(struct path_table_entry);
		path_entry += entry->len_id + (entry->len_id % 2);
		if (++cur == table_id)
			break;
	}
	if (path_entry > buffer + path_table_size)
		panic("Not found");
    printf("Read entry at 0x%x\n", entry->loc_ext_attr);
	if (!read_block(entry->loc_ext_attr, 1, buffer))
		panic("Error reading block");
    struct directory_entry *dir = (struct directory_entry *)buffer;
    printf("Loc ext attr: 0x%x\n", dir->loc_ext_attr);
	if (!read_block(dir->loc_ext_attr, 1, buffer))
		panic("Error reading block");

	struct record_attr *record = (struct record_attr *)buffer;
    printf("Size of record :%d\n", sizeof(struct record_attr));
    printf("Record format: %x\n", record->record_format);
	printf("\n");
}

void print_primary_volume_desc()
{
	printf("Primary volume descriptor:\n");
	printf("Std ID: %s\n", primary.std_id);
	printf("Vol ID: %s\n", primary.vol_id);
	printf("Log block size: %d\n", BOTH_BYTE_VAL16(primary.log_blk_size));
}

void setup_filesystem(void)
{
	char *primary_buffer = (char *)&primary;
	char *path_table_buffer = (char *)&path_table;
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

	print_primary_volume_desc();

	assert(primary.type == PRIMARY_TYPE);
	assert(BOTH_BYTE_VAL16(primary.log_blk_size) == CD_BLOCK_SZ);

	if (!read_block(primary.l_path_table, 1, buffer))
		panic("Error reading block");
	memcpy(path_table_buffer, buffer, CD_BLOCK_SZ);
	path_table_size = BOTH_BYTE_VAL32(primary.path_table_bytes);

	read_path_table();
	print_children(1);

	println("Filesystem setup");
}
