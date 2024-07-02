#include "fs.h"
#include "disk/atapi.h"
#include "k/compiler.h"
#include "panic.h"
#include "serial.h"
#include "stdio.h"

#define VOLUME_BLOCK(i) (16 + i)
#define VOL_BLK_ID "CD001"

// Types
#define BOOT_RECORD_TYPE 0
#define PRIMARY_TYPE 1
#define SUPLEMENTARY_TYPE 2
#define TERMINATOR_TYPE -1

struct volume_descriptor {
	char type;
	char id[5];
	char version;
	char data[2041];
} __packed;

int strncmp(const char *s1, const char *s2, size_t n)
{
	for (size_t i = 0; i < n; i++)
		if (s1[i] != s2[i])
			return 1;
	return 0;
}

void print_descriptor(struct volume_descriptor *desc)
{
	printf("Type: %x, ID: %5s, Version: %x\n", desc->type, desc->id,
	       desc->version);
}

void setup_filesystem(void)
{
    read();
	struct volume_descriptor block_buffer = { 0 };
	char *buffer = (char *)&block_buffer;
	setup_atapi();
	int cur = 0;
	do {
		read_block(VOLUME_BLOCK(cur++), 1, buffer);
		if (strncmp(block_buffer.id, VOL_BLK_ID,
			    sizeof(VOL_BLK_ID) - 1))
			panic("Invalid filesystem (ID: 0x%5x)", block_buffer.id);
		switch (block_buffer.type) {
		case BOOT_RECORD_TYPE:
			println("Boot record");
			break;
		case PRIMARY_TYPE:
			println("Primary filesystem");
			break;
		case SUPLEMENTARY_TYPE:
			println("Secondary filesystem");
			break;
		case TERMINATOR_TYPE:
			println("Terminator descriptor");
			break;
		default:
			panic("Unknown type of descriptor (got %x)",
			      block_buffer.type);
		}
	} while (block_buffer.type != TERMINATOR_TYPE);
	println("Filesystem setup");
}
