#define ERROR_CODE 1
#define NO_ERCODE 0
#define GATE_TYPE_TASK 0x5
#define GATE_TYPE_INT 0xE
#define GATE_TYPE_TRAP 0xF

#define IRQ_LIST                                        \
	X(64, SYSTEM_CLOCK, "System clock", NO_ERCODE) /*Begin IRQs*/ \
	X(65, KEYBOARD, "Keyboard", NO_ERCODE)                    \
	X(66, CASCADE, "Cascade", NO_ERCODE)                     \
	X(67, COM2, "COM2", NO_ERCODE)                        \
	X(68, COM1, "COM1", NO_ERCODE)                        \
	X(69, LPT2, "LPT2", NO_ERCODE)                        \
	X(70, FLOPPY_DISK, "Floppy disk", NO_ERCODE)                 \
	X(71, LPT1, "LPT1", NO_ERCODE)                        \
	X(72, PARALLEL_PORT, "Parallel port", NO_ERCODE)               \
	X(73, CRT_CONTROLLER, "CRT controller", NO_ERCODE)              \
	X(74, GENERIC_HARD_DISK, "Generic hard disk", NO_ERCODE)           \
	X(75, GENERIC_FLOPPY_DISK, "Generic floppy disk", NO_ERCODE)         \
	X(76, GENERIC_SERIAL_PORT, "Generic serial port", NO_ERCODE)         \
	X(77, GENERIC_PARALLEL_PORT, "Generic parallel port", NO_ERCODE)       \
	X(78, IDE_CONTROLLER, "IDE controller", NO_ERCODE)              \
	X(79, FLOPPY_DISK_CONTROLLER, "Floppy disk controller", NO_ERCODE)      \

#define ISR_LIST                                          \
	X(0, DIVIDE_ERROR, "Divide error", NO_ERCODE)                   \
	X(1, DEBUG, "Debug", NO_ERCODE)                          \
	X(2, NON_MASKABLE_INTERRUPT, "Non-maskable interrupt", NO_ERCODE)         \
	X(3, BREAKPOINT, "Breakpoint", NO_ERCODE)                     \
	X(4, OVERFLOW, "Overflow", NO_ERCODE)                       \
	X(5, BOUND_RANGE_EXCEEDED, "Bound range exceeded", NO_ERCODE)           \
	X(6, INVALID_OPCODE, "Invalid opcode", NO_ERCODE)                 \
	X(7, DEVICE_NOT_AVAILABLE, "Device not available", NO_ERCODE)           \
	X(8, DOUBLE_FAULT, "Double fault", ERROR_CODE)                  \
	X(9, COPROCESSOR_SEGMENT_OVERRUN, "Coprocessor segment overrun", NO_ERCODE)    \
	X(10, INVALID_TSS, "Invalid TSS", ERROR_CODE)                  \
	X(11, SEGMENT_NOT_PRESENT, "Segment not present", ERROR_CODE)          \
	X(12, STACK_SEGMENT_FAULT, "Stack segment fault", ERROR_CODE)          \
	X(13, GENERAL_PROTECTION_FAULT, "General protection fault", ERROR_CODE)     \
	X(14, PAGE_FAULT, "Page fault", ERROR_CODE)                   \
	X(15, UNKNOWN_INTERRUPT, "Unknown interrupt", NO_ERCODE)             \
	X(16, COPROCESSOR_FAULT, "Coprocessor fault", NO_ERCODE)             \
	X(17, ALIGNMENT_CHECK, "Alignment check", NO_ERCODE)               \
	X(18, MACHINE_CHECK, "Machine check", NO_ERCODE)                 \
	X(19, SIMD_FLOATING_POINT_EXCEPTION, "SIMD floating-point exception", NO_ERCODE) \
	X(20, VIRTUALIZATION_EXCEPTION, "Virtualization exception", NO_ERCODE)      \
	X(21, CONTROL_PROTECTION_EXCEPTION, "Control protection exception", NO_ERCODE)  \
	X(128, CUSTOM_SYSCALL, "Custom Syscall", NO_ERCODE) /* Syscall */
