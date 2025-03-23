#include "iso_driver.h"
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
static int root_dir = -1;

/*
 * Get the next file in the directory
 * name is the relative path to the file
 * returns a pointer to the directory entry
 */
struct iso_dir *get_file(int block, char **name)
{
	char *next_name = *name;
	while (*next_name && *next_name != '/')
		next_name++;
	int name_len = next_name - *name;
	read_block(block, 1, buffer);
	char *cur_buffer = buffer;
	struct iso_dir *dir = (struct iso_dir *)cur_buffer;
	while (1) {
		if (dir->dir_size == 0)
			return NULL;
		if (!strncasecmp(dir->idf, *name, name_len)) {
			*name = next_name + 1;
			return dir;
		}
		cur_buffer += dir->dir_size;
		dir = (struct iso_dir *)cur_buffer;
	}
}

int find(char *name, u32 *size)
{
	while (name[0] == '/' && *name)
		name++;
	int cur_dir = root_dir;
	while (1) {
		struct iso_dir *dir = get_file(cur_dir, &name);
		if (!dir) {
			printf("No such file or directory\n");
			return 0;
		}
		cur_dir = dir->data_blk.le;
		if (!(dir->type & ISO_FILE_ISDIR)) {
			*size = dir->file_size.le;
			return dir->data_blk.le;
		}
	}
}

