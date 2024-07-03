#include "fs.h"
#include "assert.h"
#include "disk/atapi.h"
#include "k/atapi.h"
#include "k/iso9660.h"
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

static char buffer[CD_BLOCK_SZ];
static struct iso_prim_voldesc primary;
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
	int i = 1;
	while (path_entry < buffer + path_table_size) {
		struct iso_path_table_le *entry =
			(struct iso_path_table_le *)path_entry;
		for (int j = 0; j < entry->idf_len; j++)
			printf("%c", entry->idf[j]);
		printf(" (%d) POSITION: %d LENGTH: %x PARENT: %d\n", i++,
		       entry->data_blk, entry->ext_size, entry->parent_dir);
		path_entry += sizeof(struct iso_path_table_le);
		path_entry += entry->idf_len + (entry->idf_len % 2);
	}
}

int process_entry(int parent, char *name, char **cur_entry)
{
	char *entry_ptr = *cur_entry;
	struct iso_path_table_le *entry;
	int id = 1;
	while (entry_ptr < buffer + path_table_size) {
		entry = (struct iso_path_table_le *)entry_ptr;
		if (entry->parent_dir == parent &&
		    !strncasecmp(entry->idf, name, entry->idf_len)) {
			*cur_entry = (char *) entry;
			return id;
		}
		entry_ptr += sizeof(struct iso_path_table_le);
		entry_ptr += entry->idf_len + (entry->idf_len % 2);
		id++;
	}
	return -1;
}

void load_file(int start, int end)
{
	printf("Loading file from %d to %d\n", start, end);
	int i = 0;
	int block = start;
	while (block <= end) {
		if (i++ == 40)
			break;
		if (!read_block(block++, 1, buffer))
			panic("Error reading block");
		struct iso_dir *dir = (struct iso_dir *)buffer;
		printf("%s\n", dir->idf);
		printf("%d\n", dir->type);
	}
}

void print_primary_volume_desc()
{
	printf("Primary volume descriptor:\n");
	printf("Std ID: %s\n", primary.std_identifier);
	printf("Vol ID: %s\n", primary.vol_idf);
	printf("Log block size: %d\n", primary.vol_blk_size.le);
}


void find(char *name)
{
	char *cur_name = name + 1;
	char *next_name = name + 1;
	int parent = 1;
	char *cur_entry = buffer;
	while (1) {
		while (*next_name && *next_name != '/')
			next_name++;
		if (!*next_name)
			break;
		*next_name = '\0';
		parent = process_entry(parent, cur_name, &cur_entry);
		if (parent == -1) {
            printf("Error reading file\n");
            return;
		}
		next_name++;
		cur_name = next_name;
	}
	struct iso_path_table_le *entry = (struct iso_path_table_le *)cur_entry;
    printf("CUR %x\n", entry->data_blk);
    u8 size = entry->ext_size;
    read_block(entry->data_blk, 1, buffer);
    struct iso_dir *file = (struct iso_dir *)buffer;
    char *cur_buffer = buffer;
    while (1)
    {
        if (cur_buffer < buffer + size)
        {
            printf("Error reading file\n");
            break;
        }
        if (!strncmp(file->idf, cur_name, file->idf_len))
            break;
        cur_buffer += file->dir_size;
        file = (struct iso_dir *)cur_buffer;
    }

	printf("Found %s at %d\n", cur_name, file->data_blk.le);
}

void setup_filesystem(void)
{
	char *primary_buffer = (char *)&primary;
	char *path_table_buffer = (char *)&path_table;
	while (!setup_atapi())
		;
	int cur = 0;
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

	assert(primary.vol_desc_type == PRIMARY_TYPE);
	assert(primary.vol_blk_size.le == CD_BLOCK_SZ);

	if (!read_block(primary.le_path_table_blk, 1, buffer))
		panic("Error reading block");
	memcpy(path_table_buffer, buffer, CD_BLOCK_SZ);
	path_table_size = primary.path_table_size.le;

	read_path_table();

	find("/bin/hunter");

	println("Filesystem setup");
}
