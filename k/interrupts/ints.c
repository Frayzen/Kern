#include "ints.h"
#include "consts.h"
#include "serial.h"

#define OFFSET_LOW(Offset) ((0xFFFF & Offset))
#define OFFSET_HIGH(Offset) ((0xFFFF0000 & Offset) >> 16)
#define GET_FUNCTION_POINTER(Offset) ((void (*)(void))(Offset))

#define IDT_ENTRIES 256
static gate_descriptor gates[IDT_ENTRIES] = { 0 };
static idt_descriptor idt_holder;

extern void isr_default(void); // divide error
extern void isr0(void); // divide error
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void); // page fault
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

extern void isr64(void); // system clock
extern void isr65(void); // keyboard

/**
 * @brief Create a gate
 * @param[in] type: the type of the gate
 * @param[in] privilege: the ring privilege
 * @return gate_access
 */
gate_flags create_flags(unsigned int type, unsigned int privilege)
{
	gate_flags gate = {
		.type = type,
		.privilege = privilege,
		.present = 1,
	};
	return gate;
}
/**
 * @brief Set a gate
 * @param[in] id: the id of the gate
 * @param[in] offset: the address of the instruction to execute
 * @param[in] selector: the selector of the gate in the GDT
 * @param[in] access: the access of the gate
 * @return void
 */
void set_gate(unsigned int id, void *function, unsigned int selector,
	      gate_flags access)
{
	unsigned int offset = (unsigned int)function;
	gate_descriptor gate_desc = {
		.offset_low = OFFSET_LOW(offset),
		.selector = selector,
		.offset_high = OFFSET_HIGH(offset),
		.access = access,
	};
	gates[id] = gate_desc;
}

void print_gate(gate_descriptor gate_desc)
{
	print("Offset: ");
	print_uint(gate_desc.offset_high, 4);
	print_uint(gate_desc.offset_low, 4);
	print("  Selector: ");
	print_uint(gate_desc.selector, 4);
	print("  Type: ");
	print_uint(gate_desc.access.type, 1);
	print("  Privilege: ");
	print_uint(gate_desc.access.privilege, 2);
	print("  Present: ");
	print_uint(gate_desc.access.present, 1);
	println("");
}

void set_gates()
{
	gate_flags gate = create_flags(GATE_TYPE_INT, SEG_KERNEL_PRVLG);

	for (int i = 0; i < IDT_ENTRIES; i++)
		set_gate(i, isr_default, i, gate);
	set_gate(0, isr0, 0x8, gate);
    set_gate(1, isr1, 0x8, gate);
    set_gate(2, isr2, 0x8, gate);
    set_gate(3, isr3, 0x8, gate);
    set_gate(4, isr4, 0x8, gate);
    set_gate(5, isr5, 0x8, gate);
    set_gate(6, isr6, 0x8, gate);
    set_gate(7, isr7, 0x8, gate);
    set_gate(8, isr8, 0x8, gate);
    set_gate(9, isr9, 0x8, gate);
    set_gate(10, isr10, 0x8, gate);
    set_gate(11, isr11, 0x8, gate);
    set_gate(12, isr12, 0x8, gate);
    set_gate(13, isr13, 0x8, gate);
    set_gate(14, isr14, 0x8, gate);
    set_gate(15, isr15, 0x8, gate);
    set_gate(16, isr16, 0x8, gate);
    set_gate(17, isr17, 0x8, gate);
    set_gate(18, isr18, 0x8, gate);
    set_gate(19, isr19, 0x8, gate);
    set_gate(20, isr20, 0x8, gate);
    set_gate(21, isr21, 0x8, gate);
    set_gate(22, isr22, 0x8, gate);
    set_gate(23, isr23, 0x8, gate);
    set_gate(24, isr24, 0x8, gate);
    set_gate(25, isr25, 0x8, gate);
    set_gate(26, isr26, 0x8, gate);
    set_gate(27, isr27, 0x8, gate);
    set_gate(28, isr28, 0x8, gate);
    set_gate(29, isr29, 0x8, gate);
    set_gate(30, isr30, 0x8, gate);
    set_gate(31, isr31, 0x8, gate);

	set_gate(64, isr64, 0x8, gate);
	set_gate(65, isr65, 0x8, gate);
}

void setup_idt(void)
{
	setup_pic();
	println("Setting up IDT...");
	set_gates();
	/* print_gate(gates[0]); */
	idt_holder.limit = IDT_ENTRIES * sizeof(idt_descriptor) - 1;
	idt_holder.base = (unsigned int)&gates;
	asm volatile("lidt %0"
		     : /* no output */
		     : "m"(idt_holder)
		     : "memory");
	println("IDT loaded");
}
