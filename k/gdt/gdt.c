#include "gdt.h"
#include "k/compiler.h"
#include "serial.h"

// DATA SEGMENT
#define SEG_DATA 0x0
#define SEG_DATA_WR 0x2

// CODE SEGMENT
#define SEG_CODE 0x8
#define SEG_ACCESS 0x1
#define SEG_CODE_RD 0x2

#define KERNEL_PRVLG 0x0
#define USER_PRVLG 0x3

struct segment_descriptor {
	unsigned int limit_low : 16;
	unsigned int base_low : 16;
	unsigned int base_mid : 8;
	unsigned int segment_type : 4; // ONE OF THE SEG_(CODE/DATA)_XXXX
	unsigned int desc_type : 1; // (0 = system, 1 = code data)
	unsigned int privilege : 2; // (ring of privilege)
	unsigned int present : 1;
	unsigned int limit_high : 4;
	unsigned int available : 1; //AVL
	unsigned int segment_size : 1; // 64 bit code segment
	unsigned int operation_size : 1; // (0=16 bits, 1=32bits)
	unsigned int granularity : 1;
	unsigned int base_high : 8;
} __packed;

struct gdt_descriptor {
	unsigned int limit : 16;
	unsigned int base : 32;
} __packed;

#define NULL_DESC 0
#define KERNEL_CODE_DESC 1
#define KERNEL_DATA_DESC 2
#define USER_CODE_DESC 3
#define USER_DATA_DESC 4

#define DESC_NB (sizeof(descriptors) / sizeof(struct segment_descriptor))

static struct segment_descriptor descriptors[] = {
    [NULL_DESC] = {
        .limit_low = 0,
        .base_low = 0,
        .base_mid = 0,
        .segment_type = SEG_DATA,
        .desc_type = 0,
        .privilege = 0,
        .present = 0,
        .limit_high = 0,
        .available = 0,
        .segment_size = 0,
        .operation_size = 0,
        .granularity = 0,
        .base_high = 0,
    },
    [KERNEL_CODE_DESC] = {
        .base_low = 0,
        .base_mid = 0,
        .segment_type = SEG_CODE | SEG_CODE_RD,
        .desc_type = 1,
        .privilege = KERNEL_PRVLG,
        .present = 1,
        .limit_low = 0xFFFF,
        .limit_high = 0xF,
        .available = 0,
        .segment_size = 0,
        .operation_size = 1,
        .granularity = 1,
        .base_high = 0,
    },
    [KERNEL_DATA_DESC] = {
        .base_low = 0x0000,
        .base_mid = 0,
        .segment_type = SEG_DATA | SEG_DATA_WR,
        .desc_type = 1,
        .privilege = KERNEL_PRVLG,
        .present = 1,
        .limit_low = 0xFFFF,
        .limit_high = 0xF,
        .available = 0,
        .segment_size = 0,
        .operation_size = 1,
        .granularity = 1,
        .base_high = 0,
    },
    [USER_CODE_DESC] = {
        .base_low = 0x0000,
        .base_mid = 0,
        .segment_type = SEG_CODE | SEG_CODE_RD,
        .desc_type = 1,
        .privilege = USER_PRVLG,
        .present = 1,
        .limit_low = 0xFFFF,
        .limit_high = 0xF,
        .available = 0,
        .segment_size = 0,
        .operation_size = 1,
        .granularity = 1,
        .base_high = 0,
    },
    [USER_DATA_DESC] = {
        .base_low = 0,
        .base_mid = 0,
        .segment_type = SEG_DATA | SEG_DATA_WR,
        .desc_type = 1,
        .privilege = USER_PRVLG,
        .present = 1,
        .limit_low = 0xFFFF,
        .limit_high = 0xF,
        .available = 0,
        .segment_size = 0,
        .operation_size = 1,
        .granularity = 1,
        .base_high = 0,
    },
};

extern void gdtFlush(void);

void setup_gdt()
{
	println("Setting up GDT...");
	struct gdt_descriptor gdt_holder;
	gdt_holder.limit = sizeof(descriptors) - 1;
	gdt_holder.base = (unsigned int)&descriptors;
	asm volatile("lgdt %0"
		     : /* no output */
		     : "m"(gdt_holder)
		     : "memory");
	gdtFlush();
	println("GDT set up");
}

void print_gdt(void)
{
	print("Size of a segment descriptor: ");
	print_uint(sizeof(struct segment_descriptor), 1);
	println("");
	for (unsigned int i = 0; i < DESC_NB; i++) {
		println("===========================");
		println("Base: ");
		print("HIGH ");
		print_uint(descriptors[i].base_high, 1);
		print("  MID ");
		print_uint(descriptors[i].base_mid, 1);
		print("  LOW ");
		print_uint(descriptors[i].base_low, 2);
		println();
		println("Limit: ");
		print("HIGH ");
		print_uint(descriptors[i].limit_low, 2);
		println();
	}
}
