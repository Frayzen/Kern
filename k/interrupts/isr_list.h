#define ERROR_CODE 1
#define NO_ERCODE 0

#define IRQ_LIST                                                      \
	X(64, SYSTEM_CLOCK, "System clock", NO_ERCODE) /*Begin IRQs*/ \
	X(65, KEYBOARD, "Keyboard", NO_ERCODE)                        \
	X(66, NVME_ADMIN_QUEUE, "NVMe Admin Queue", NO_ERCODE)        \
	X(67, NVME_IO_QUEUE, "NVMe IO Queue", NO_ERCODE)              \
	X(78, PRIM_IDE_CONT, "Primary IDE Controller", NO_ERCODE)     \
	X(79, SEC_IDE_CONT, "Secondary IDE Controller", NO_ERCODE)    \

#define ISR_LIST                                                               \
	X(0, DIVIDE_ERROR, "Divide error", NO_ERCODE)                          \
	X(1, DEBUG, "Debug", NO_ERCODE)                                        \
	X(2, NON_MASKABLE_INTERRUPT, "Non-maskable interrupt", NO_ERCODE)      \
	X(3, BREAKPOINT, "Breakpoint", NO_ERCODE)                              \
	X(4, OVERFLOW, "Overflow", NO_ERCODE)                                  \
	X(5, BOUND_RANGE_EXCEEDED, "Bound range exceeded", NO_ERCODE)          \
	X(6, INVALID_OPCODE, "Invalid opcode", NO_ERCODE)                      \
	X(7, DEVICE_NOT_AVAILABLE, "Device not available", NO_ERCODE)          \
	X(8, DOUBLE_FAULT, "Double fault", ERROR_CODE)                         \
	X(9, COPROCESSOR_SEGMENT_OVERRUN, "Coprocessor segment overrun",       \
	  NO_ERCODE)                                                           \
	X(10, INVALID_TSS, "Invalid TSS", ERROR_CODE)                          \
	X(11, SEGMENT_NOT_PRESENT, "Segment not present", ERROR_CODE)          \
	X(12, STACK_SEGMENT_FAULT, "Stack segment fault", ERROR_CODE)          \
	X(13, GENERAL_PROTECTION_FAULT, "General protection fault",            \
	  ERROR_CODE)                                                          \
	X(14, PAGE_FAULT, "Page fault", ERROR_CODE)                            \
	X(15, UNKNOWN_INTERRUPT, "Unknown interrupt", NO_ERCODE)               \
	X(16, COPROCESSOR_FAULT, "Coprocessor fault", NO_ERCODE)               \
	X(17, ALIGNMENT_CHECK, "Alignment check", NO_ERCODE)                   \
	X(18, MACHINE_CHECK, "Machine check", NO_ERCODE)                       \
	X(19, SIMD_FLOATING_POINT_EXCEPTION, "SIMD floating-point exception",  \
	  NO_ERCODE)                                                           \
	X(20, VIRTUALIZATION_EXCEPTION, "Virtualization exception", NO_ERCODE) \
	X(21, CONTROL_PROTECTION_EXCEPTION, "Control protection exception",    \
	  NO_ERCODE)                                                           \
	X(128, CUSTOM_SYSCALL, "Custom Syscall", NO_ERCODE) /* Syscall */
