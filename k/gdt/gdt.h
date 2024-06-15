#ifndef GDT_H
#define GDT_H

#define SEG_DATA_RD 0x00 // Read-Only
#define SEG_DATA_RDA 0x01 // Read-Only, accessed
#define SEG_DATA_RDWR 0x02 // Read/Write
#define SEG_DATA_RDWRA 0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD 0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA 0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD 0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX 0x08 // Execute-Only
#define SEG_CODE_EXA 0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD 0x0A // Execute/Read
#define SEG_CODE_EXRDA 0x0B // Execute/Read, accessed
#define SEG_CODE_EXC 0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA 0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC 0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA 0x0F // Execute/Read, conforming, accessed

#define MAX_DESCRIPTORS 16

typedef struct {
	unsigned int segment_type : 4; // ONE OF THE SEG_(CODE/DATA)_XXXX
	unsigned int desc_type : 1; // (0 = system, 1 = code data)
	unsigned int privilege : 2; // (ring of privilege)
	unsigned int present : 1;
} __attribute__((packed)) segment_access;

typedef struct {
	unsigned int limit_high : 4;
	unsigned int available : 1; //AVL
	unsigned int segment_size : 1; // 64 bit code segment
	unsigned int operation_size : 1; // (0=16 bits, 1=32bits)
	unsigned int granularity : 1;
} __attribute__((packed)) segment_flags;

typedef struct {
	unsigned int limit_low : 16;
	unsigned int base_low : 16;
	unsigned int base_mid : 8;
	segment_access access_byte __attribute__((packed));
	segment_flags flags __attribute__((packed));
	unsigned int base_high : 8;
} __attribute__((packed)) segment_descriptor;

typedef struct {
	unsigned int limit : 16;
    unsigned int base : 32;
} __attribute__((packed)) gdt_descriptor;

void setup_gdt();
void testFn(unsigned int val, unsigned int size);

#endif /* !GDT_H */
