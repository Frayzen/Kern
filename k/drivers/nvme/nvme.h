#ifndef NVME_H
#define NVME_H

#include "drivers/pci/pci.h"
#include "k/compiler.h"
#include "k/types.h"

struct nvme_queue {
	u64 address;
	u64 size;
} __packed;

struct nvme_device {
	u16 c_head;
	u16 s_tail;

	u32 next_command_id;

	struct pci_device pci;
	u64 base_addr;
	u64 capability_stride;
	// admin queues
	struct nvme_queue adm_cmpl_q;
	struct nvme_queue adm_subm_q;
	struct nvme_queue io_cmpl_q;
	struct nvme_queue io_subm_q;
} __packed;

// 64 bytes, arranged in 16 DWORDs (1 DWORDS is 4 bytes)
struct nvme_command_entry {
	// DWORD 0
	u8 opcode;
	u8 fuse_op : 2; // 0 indicates normal operation
	u8 __reserved0 : 4;
	u8 prp_or_sgl : 2; // 0 indicates prp
	u16 command_id; // This is put in the completion queue entry
	// DWORD 1
	u32 nsid; // If n/a, set to 0.
	// DWORD 2-3
	u64 __reserved1;
	// DWORD 4-5
	u64 metadat_ptr;
	// DWORD 6-9
	u32 prp1;
	u32 prp2;
	// DWORD 10-15 - COMMAND SPECIFIC
	u32 command_specific[6];
} __packed;

// 16 bytes
struct nvme_completion {
	u32 command_specific;
	u32 reserved__0;
	u16 queue_head_ptr;
	u16 queue_id;
	u16 command_id;
	u8 phase_bit : 1;
	u16 status : 15; // 0 on sucess
} __packed;

#define QUEUE_SIZE_POW 2
#define QUEUE_SIZE (1 << QUEUE_SIZE_POW)

#define NVME_CLASS_CODE 0x1
#define NVME_SUBCLASS 0x8

#define OPCODE_IO_SUBMISSION_QUEUE_CREATE 0x01
#define OPCODE_IO_COMPLETION_QUEUE_CREATE 0x05

#define FLAG_CONTIGUOUS_QUEUE (1 << 0)
#define FLAG_ENABLE_INTS (1 << 1)

// REGISTERS (to append to base_addr)
#define NVME_CAP 0x0 // capabilities
#define NVME_VS 0x8 // version
#define NVME_INTMS 0xC // interrupt mask set
#define NVME_INTMC 0x10 // interrupt mask clear
#define NVME_CC 0x14 // controller configuration
#define NVME_CST 0x1C // controller status
#define NVME_AQA 0x24 // admin queue attributes
#define NVME_ASQ 0x28 // admin submission queue reg
#define NVME_ACQ 0x30 // admin completion queue reg

// Y is the doorbell stride, specified in the controller capabilities register
#define NVME_SQxTDBL(X, Y) \
	(0x1000 + (2 * (X)) * (4 << Y)) // Submission queue X tail doorbell
#define NVME_CQxHDBL(X, Y) \
	(0x1000 + (2 * (X) + 1) * (4 << Y)) //Completion queue X head doorbell

void init_nvme(void);

#endif /* !NVME_H */
