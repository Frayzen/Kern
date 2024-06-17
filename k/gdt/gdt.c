#include "gdt.h"
#include "consts.h"
#include "gdt/tss.h"
#include "serial.h"

#define BASE_LOW(Base) ((0xFFFF & Base))
#define BASE_MID(Base) ((0xFF0000 & Base) >> 16)
#define BASE_HIGH(Base) ((0xFF000000 & Base) >> 24)
#define LIMIT_LOW(Limit) ((0x0000FFFF & Limit))
#define LIMIT_HIGH(Limit) ((0x000F0000 & Limit) >> 16)

static int desc_nb = 0;
static segment_descriptor descriptors[MAX_DESCRIPTORS];
gdt_descriptor gdt_holder = {};

/**
 * @brief Tool to create a build_flag
 * @param[in] available: available for system software
 * @param[in] seg_size: 64-bit code segment
 * @param[in] op_size: default operation size (0 = 16-bit segment, 1 = 32-bit segment)
 * @param[in] granularity: is granularity enable
 */
inline segment_flags build_flag(int available, int seg_size, int op_size,
				int granularity)
{
	return ((segment_flags){ .available = (available),
				 .segment_size = (seg_size),
				 .operation_size = (op_size),
				 .granularity = (granularity) });
}
/**
 * @brief Tool to create a build_access
 * @param[in] seg_type: the read/write access. one of the SEG_(CODE/DATA)_XXX
 * @param[in] desc_type: 0 if system or 1 for code and data
 * @param[in] privilege: the ring privilege
 * @param[in] present: is the segment present
 */
inline segment_access build_access(int seg_type, int desc_type, int privilege,
				   int present)
{
	return ((segment_access){ .segment_type = (seg_type),
				  .desc_type = (desc_type),
				  .privilege = (privilege),
				  .present = (present) });
}

void append_descriptor(unsigned int base, unsigned int limit,
		       segment_access access_byte, segment_flags flags)
{
	flags.limit_high = LIMIT_HIGH(limit);
	segment_descriptor new_desc = {
		.flags = flags,
		.access_byte = access_byte,
		.base_low = BASE_LOW(base),
		.base_mid = BASE_MID(base),
		.base_high = BASE_HIGH(base),
		.limit_low = LIMIT_LOW(limit),
	};
	descriptors[desc_nb++] = new_desc;
}

extern void gdtFlush(void);

void print_access(segment_access access_byte)
{
	print_uint(access_byte.present, 1);
	print_uint(access_byte.segment_type, 1);
	print_uint(access_byte.desc_type, 1);
	print_uint(access_byte.privilege, 1);
}

void print_flags(segment_flags flags)
{
	print_uint(flags.available, 1);
	print_uint(flags.segment_size, 1);
	print_uint(flags.operation_size, 1);
	print_uint(flags.granularity, 1);
}

void print_gdt(void)
{
	print("Size of a segment descriptor: ");
	print_uint(sizeof(segment_descriptor), 1);
	println("");
	for (int i = 0; i < desc_nb; i++) {
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
		print_uint(descriptors[i].flags.limit_high, 1);
		print("  LOW ");
		print_uint(descriptors[i].limit_low, 2);
		println();
		println("Access: ");
		print_access(descriptors[i].access_byte);
		println();
		println("Flags: ");
		print_flags(descriptors[i].flags);
		println();
	}
}

void append_tss()
{
	tss_entry *tss = setup_tss();
	append_descriptor((unsigned int)tss, sizeof(tss_entry) - 1,
			  build_access(SEG_CODE_EXA, 0, 0, 1),
			  (segment_flags){ 0 });
}

void setup_gdt()
{
	println("Setting up GDT...");
	// NULL DESCRIPTOR
	append_descriptor(0, 0, (segment_access){ 0 }, (segment_flags){ 0 });
	// KERNEL MODE CODE SEGMENT
	append_descriptor(0, 0xFFFFF,
			  build_access(SEG_CODE_EXRD, 1, SEG_KERNEL_PRVLG, 1),
			  build_flag(0, 0, 1, 1));
	append_descriptor(0, 0xFFFFF,
			  build_access(SEG_DATA_RDWR, 1, SEG_KERNEL_PRVLG, 1),
			  build_flag(0, 0, 1, 1));
	append_descriptor(0, 0xFFFFF,
			  build_access(SEG_CODE_EXRD, 1, SEG_USER_PRVLG, 1),
			  build_flag(0, 0, 1, 1));
	append_descriptor(0, 0xFFFFF,
			  build_access(SEG_DATA_RDWR, 1, SEG_USER_PRVLG, 1),
			  build_flag(0, 0, 1, 1));
	append_tss();
	/* print_gdt(); */
	gdt_holder.limit = (sizeof(segment_descriptor) * desc_nb) - 1;
	gdt_holder.base = (unsigned int)&descriptors;
	asm volatile("lgdt %0"
		     : /* no output */
		     : "m"(gdt_holder)
		     : "memory");
	gdtFlush();
	println("GDT loaded\n");
}
