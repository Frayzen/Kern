#define ERROR_CODE 1
#define NO_ERCODE 0
#define GATE_TYPE_TASK 0x5
#define GATE_TYPE_INT 0xE
#define GATE_TYPE_TRAP 0xF

#define IRQ_LIST                                                      \
	X(64, SYSTEM_CLOCK, "System clock", NO_ERCODE) /*Begin IRQs*/ \
	X(65, KEYBOARD, "Keyboard", NO_ERCODE)                        \
	X(66, NVME_ADMIN_QUEUE, "NVMe Admin Queue", NO_ERCODE)        \
	X(67, NVME_IO_QUEUE, "NVMe IO Queue", NO_ERCODE)              \
	X(68, UNUSED_68, "UNUSED", NO_ERCODE)                         \
	X(69, UNUSED_69, "UNUSED", NO_ERCODE)                         \
	X(70, UNUSED_70, "UNUSED", NO_ERCODE)                         \
	X(71, UNUSED_71, "UNUSED", NO_ERCODE)                         \
	X(72, UNUSED_72, "UNUSED", NO_ERCODE)                         \
	X(73, UNUSED_73, "UNUSED", NO_ERCODE)                         \
	X(74, UNUSED_74, "UNUSED", NO_ERCODE)                         \
	X(75, UNUSED_75, "UNUSED", NO_ERCODE)                         \
	X(76, UNUSED_76, "UNUSED", NO_ERCODE)                         \
	X(77, UNUSED_77, "UNUSED", NO_ERCODE)                         \
	X(78, PRIM_IDE_CONT, "Primary IDE Controller", NO_ERCODE)     \
	X(79, SEC_IDE_CONT, "Secondary IDE Controller", NO_ERCODE)    \
	X(80, UNUSED_80, "UNUSED", NO_ERCODE)                         \
	X(81, UNUSED_81, "UNUSED", NO_ERCODE)                         \
	X(82, UNUSED_82, "UNUSED", NO_ERCODE)                         \
	X(83, UNUSED_83, "UNUSED", NO_ERCODE)                         \
	X(84, UNUSED_84, "UNUSED", NO_ERCODE)                         \
	X(85, UNUSED_85, "UNUSED", NO_ERCODE)                         \
	X(86, UNUSED_86, "UNUSED", NO_ERCODE)                         \
	X(87, UNUSED_87, "UNUSED", NO_ERCODE)                         \
	X(88, UNUSED_88, "UNUSED", NO_ERCODE)                         \
	X(89, UNUSED_89, "UNUSED", NO_ERCODE)                         \
	X(90, UNUSED_90, "UNUSED", NO_ERCODE)                         \
	X(91, UNUSED_91, "UNUSED", NO_ERCODE)                         \
	X(92, UNUSED_92, "UNUSED", NO_ERCODE)                         \
	X(93, UNUSED_93, "UNUSED", NO_ERCODE)                         \
	X(94, UNUSED_94, "UNUSED", NO_ERCODE)                         \
	X(95, UNUSED_95, "UNUSED", NO_ERCODE)                         \
	X(96, UNUSED_96, "UNUSED", NO_ERCODE)                         \
	X(97, UNUSED_97, "UNUSED", NO_ERCODE)                         \
	X(98, UNUSED_98, "UNUSED", NO_ERCODE)                         \
	X(99, UNUSED_99, "UNUSED", NO_ERCODE)                         \
	X(100, UNUSED_100, "UNUSED", NO_ERCODE)                       \
	X(101, UNUSED_101, "UNUSED", NO_ERCODE)                       \
	X(102, UNUSED_102, "UNUSED", NO_ERCODE)                       \
	X(103, UNUSED_103, "UNUSED", NO_ERCODE)                       \
	X(104, UNUSED_104, "UNUSED", NO_ERCODE)                       \
	X(105, UNUSED_105, "UNUSED", NO_ERCODE)                       \
	X(106, UNUSED_106, "UNUSED", NO_ERCODE)                       \
	X(107, UNUSED_107, "UNUSED", NO_ERCODE)                       \
	X(108, UNUSED_108, "UNUSED", NO_ERCODE)                       \
	X(109, UNUSED_109, "UNUSED", NO_ERCODE)                       \
	X(110, UNUSED_110, "UNUSED", NO_ERCODE)                       \
	X(111, UNUSED_111, "UNUSED", NO_ERCODE)                       \
	X(112, UNUSED_112, "UNUSED", NO_ERCODE)                       \
	X(113, UNUSED_113, "UNUSED", NO_ERCODE)                       \
	X(114, UNUSED_114, "UNUSED", NO_ERCODE)                       \
	X(115, UNUSED_115, "UNUSED", NO_ERCODE)                       \
	X(116, UNUSED_116, "UNUSED", NO_ERCODE)                       \
	X(117, UNUSED_117, "UNUSED", NO_ERCODE)                       \
	X(118, UNUSED_118, "UNUSED", NO_ERCODE)                       \
	X(119, UNUSED_119, "UNUSED", NO_ERCODE)                       \
	X(120, UNUSED_120, "UNUSED", NO_ERCODE)                       \
	X(121, UNUSED_121, "UNUSED", NO_ERCODE)                       \
	X(122, UNUSED_122, "UNUSED", NO_ERCODE)                       \
	X(123, UNUSED_123, "UNUSED", NO_ERCODE)                       \
	X(124, UNUSED_124, "UNUSED", NO_ERCODE)                       \
	X(125, UNUSED_125, "UNUSED", NO_ERCODE)                       \
	X(126, UNUSED_126, "UNUSED", NO_ERCODE)                       \
	X(127, UNUSED_127, "UNUSED", NO_ERCODE)

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
