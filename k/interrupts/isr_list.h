#define ERROR_CODE 1
#define NO_ERCODE 0
#define GATE_TYPE_TASK 0x5
#define GATE_TYPE_INT 0xE
#define GATE_TYPE_TRAP 0xF
#define ISR_LIST                                          \
	X(0, "Divide error", NO_ERCODE)                   \
	X(1, "Debug", NO_ERCODE)                          \
	X(2, "Non-maskable interrupt", NO_ERCODE)         \
	X(3, "Breakpoint", NO_ERCODE)                     \
	X(4, "Overflow", NO_ERCODE)                       \
	X(5, "Bound range exceeded", NO_ERCODE)           \
	X(6, "Invalid opcode", NO_ERCODE)                 \
	X(7, "Device not available", NO_ERCODE)           \
	X(8, "Double fault", ERROR_CODE)                  \
	X(9, "Coprocessor segment overrun", NO_ERCODE)    \
	X(10, "Invalid TSS", ERROR_CODE)                  \
	X(11, "Segment not present", ERROR_CODE)          \
	X(12, "Stack segment fault", ERROR_CODE)          \
	X(13, "General protection fault", ERROR_CODE)     \
	X(14, "Page fault", ERROR_CODE)                   \
	X(15, "Unknown interrupt", NO_ERCODE)             \
	X(16, "Coprocessor fault", NO_ERCODE)             \
	X(17, "Alignment check", NO_ERCODE)               \
	X(18, "Machine check", NO_ERCODE)                 \
	X(19, "SIMD floating-point exception", NO_ERCODE) \
	X(20, "Virtualization exception", NO_ERCODE)      \
	X(21, "Control protection exception", NO_ERCODE)  \
	X(64, "System clock", NO_ERCODE) /*Begin IRQs*/   \
	X(65, "Keyboard", NO_ERCODE)                      \
	X(66, "Cascade", NO_ERCODE)                       \
	X(67, "COM2", NO_ERCODE)                          \
	X(68, "COM1", NO_ERCODE)                          \
	X(69, "LPT2", NO_ERCODE)                          \
	X(70, "Floppy disk", NO_ERCODE)                   \
	X(71, "LPT1", NO_ERCODE)                          \
	X(72, "Parallel port", NO_ERCODE)                 \
	X(73, "CRT controller", NO_ERCODE)                \
	X(74, "Generic hard disk", NO_ERCODE)             \
	X(75, "Generic floppy disk", NO_ERCODE)           \
	X(76, "Generic serial port", NO_ERCODE)           \
	X(77, "Generic parallel port", NO_ERCODE)         \
	X(78, "IDE controller", NO_ERCODE)                \
	X(79, "Floppy disk controller", NO_ERCODE)        \
	X(128, "Custom Syscall", NO_ERCODE) /* Syscall */
