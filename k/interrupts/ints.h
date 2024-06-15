#ifndef INTS_H
#define INTS_H

#define GATE_TYPE_TASK 0x5
#define GATE_TYPE_INT_16 0x6
#define GATE_TYPE_TRAP_16 0x7
#define GATE_TYPE_INT_32 0xE
#define GATE_TYPE_TRAP_32 0xF

typedef struct {
    unsigned int type : 4; // one of the GATE_TYPE_XXX
    unsigned int __unused : 1;
	unsigned int privilege : 2; // (ring of privilege)
    unsigned int present : 1;

} __attribute__((packed)) gate_access;

typedef struct {
    unsigned int offset_low : 16;
    unsigned int selector : 16;
    unsigned int __unused : 8;
    gate_access access;
    unsigned int offset_high : 16;
} __attribute__((packed)) gate_desc;

#endif /* !INTS_H */
