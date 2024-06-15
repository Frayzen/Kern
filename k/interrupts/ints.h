#ifndef INTS_H
#define INTS_H

#define GATE_TYPE_TASK 0x5
#define GATE_TYPE_INT 0xE
#define GATE_TYPE_TRAP 0xF

typedef struct {
    unsigned int type : 5; // one of the GATE_TYPE_XXX
	unsigned int privilege : 2; // (ring of privilege)
    unsigned int present : 1;

} __attribute__((packed)) gate_flags;

typedef struct {
    unsigned int offset_low : 16;
    unsigned int selector : 16;
    unsigned int __unused : 8;
    gate_flags access;
    unsigned int offset_high : 16;
} __attribute__((packed)) gate_descriptor;

typedef struct {
	unsigned int limit : 16;
    unsigned int base : 32;
} __attribute__((packed)) idt_descriptor;

void setup_idt(void);
void interrupt_handler(void);
void setup_pic(void);

#endif /* !INTS_H */
