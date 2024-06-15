#include "ints.h"
#include "consts.h"
#include "serial.h"

#define OFFSET_LOW(Offset) ((0x00FF & Offset))
#define OFFSET_HIGH(Offset) ((0xFF00 & Offset) >> 8)

#define IDT_ENTRIES 256
static gate_desc gates[IDT_ENTRIES];


/**
 * @brief Create a gate
 * @param[in] type: the type of the gate
 * @param[in] privilege: the ring privilege
 * @return gate_access
 */
gate_access create_gate(unsigned int type, unsigned int privilege)
{
	gate_access gate = {
		.type = type,
		.privilege = privilege,
		.present = 1,
		.__unused = 0,
	};
	return gate;
}

void set_gate(unsigned int id, unsigned int offset, unsigned int selector,
			   gate_access access)
{
	gate_desc gate_desc = {
		.offset_low = OFFSET_LOW(offset),
		.selector = selector,
		.offset_high = OFFSET_HIGH(offset),
        .access = access,
	};
	gates[id] = gate_desc;
}

void print_gate(gate_desc gate_desc)
{
    print("Offset: ");
    print_uint(gate_desc.offset_low, 2);
    print("  Selector: ");
    print_uint(gate_desc.selector, 4);
    print("  Access: ");
    print_uint(gate_desc.access.type, 2);
    print("  Privilege: ");
    print_uint(gate_desc.access.privilege, 2);
    print("  Present: ");
    print_uint(gate_desc.access.present, 1);
    println("");
}


void setup_idt(void)
{
    set_gate(0, 0, 0, create_gate(GATE_TYPE_TASK, SEG_KERNEL_PRVLG));
}
