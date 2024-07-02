#include "fs.h"
#include "disk/atapi.h"
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

int strncmp(const char *s1, const char *s2, size_t n)
{
	for (size_t i = 0; i < n; i++)
		if (s1[i] != s2[i])
			return 1;
	return 0;
}

static char buffer[2048] = { 1 };
void setup_filesystem(void)
{
	setup_atapi();
	for (int i = 0; i < 2048; i++)
		buffer[i] = 0;
	for (int i = 0; i < 5; i++)
		printf("%x ", buffer[i]);
	println("");
    printf("Pointer %x\n", buffer);
	int cur = 0;
	do {
		read_block(VOLUME_BLOCK(cur++), 1, buffer);
		for (int i = 0; i < 5; i++)
			printf("%x ", buffer[i]);
		println("");
		if (strncmp(buffer + 1, VOL_BLK_ID, sizeof(VOL_BLK_ID) - 1)) {
			for (int i = 0; i < 5; i++)
				printf("%x ", buffer[i]);
			panic("Invalid filesystem");
		}
		switch (*buffer) {
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
			panic("Unknown type of descriptor (got %x)", buffer[0]);
		}
	} while (buffer[0] != TERMINATOR_TYPE);
	println("Filesystem setup");
}
