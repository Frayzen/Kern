#include "ints.h"
#include "consts.h"
#include "serial.h"

#define OFFSET_LOW(Offset) ((0xFFFF & Offset))
#define OFFSET_HIGH(Offset) ((0xFFFF0000 & Offset) >> 16)
#define GET_FUNCTION_POINTER(Offset) ((void (*)(void))(Offset))

#define IDT_ENTRIES 256
static gate_descriptor gates[IDT_ENTRIES] = { 0 };
static idt_descriptor idt_holder;
extern void isr(void);

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
    for (unsigned int i = 0; i < IDT_ENTRIES; i++)
	    set_gate(i, isr, 0x8, gate);
}

void test()
{
	asm volatile("sti" :);
    asm volatile("int $0x0");
	int k = 1/0;
    print_uint(k, 1);
}

void setup_idt(void)
{
	setup_pic();
	println("Loading IDT...");
	read();
	println("Setting up IDT...");
	set_gates();
	print_gate(gates[0]);
	idt_holder.limit = IDT_ENTRIES * sizeof(idt_descriptor) - 1;
	idt_holder.base = (unsigned int)&gates;
	asm volatile("lidt %0"
		     : /* no output */
		     : "m"(idt_holder)
		     : "memory");
	println("IDT loaded");
	read();
    test();
}

void interrupt_handler(void)
{
	println("Interrupt handler called");
	read();
}
