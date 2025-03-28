#ifndef NVME_H
#define NVME_H

#include "drivers/pci/pci.h"
#include "k/compiler.h"
#include "k/types.h"

struct nvme_queue {
  u32 queue_id;
	u64 address;
	u64 size;
	u64 ptr; // head or tail
	volatile u32 *door_bell;
} __packed;

struct nvme_device {
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

struct nvme_controller_status {
	u8 ready : 1;
	u8 fatal_status : 1;
	u8 shutdown_status : 2;
	u8 subsys_reset_occured : 1;
	u8 process_pause : 1;
	u8 shutdown_type : 1;
	u32 reserved__ : 25;
} __packed;

struct nvme_controller_command {
  u8 enable : 1;
  u8 reserved1__ : 3;
  u8 command_set_selected : 3;
  u8 mem_page_size : 4;
  u8 arbit_mech_selected : 3;
  u8 shutdown_notif : 2;
  u8 io_subm_q_entry_size : 4;
  u8 io_compl_q_entry_size : 4;
  u8 ready_independent_of_media_enable : 1;
  u8 reserved2__ : 7;
} __packed;

struct nvme_aqa {
  u16 admin_subq_size : 12;
  u8 reserved1__ : 4;
  u16 admin_complq_size : 12;
  u8 reserved2__ : 4;
} __packed;

struct command_dword {
	u8 opcode;
	u8 fuse_op : 2; // 0 indicates normal operation
	u8 __reserved0 : 4;
	u8 prp_or_sgl : 2; // 0 indicates prp
	u16 command_id; // This is put in the completion queue entry
} __packed;

// 64 bytes ()
struct submission_q_entry {
	// DWORD 0
	struct command_dword cmd;

	// DWORD 1
	u32 nsid; // If n/a, set to 0.
	// DWORD 2-3
	u64 __reserved1;
	// DWORD 4-5
	u64 metadat_ptr;
	// DWORD 6-9
	u64 prp1;
	u64 prp2;
	// DWORD 10-15 - COMMAND SPECIFIC
	u32 command_specific[6];

} __packed;

// 16 bytes
struct completion_q_entry {
	u32 command_specific;
	u32 reserved__0;
	u16 queue_head_ptr;
	u16 queue_id;
	u16 command_id;
	u8 phase_bit : 1;
	u16 status : 15; // 0 on sucess
};

#define QUEUE_SIZE_POW 7
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

void nvme_init(void);
volatile u32 *nvme_reg(struct nvme_device *dev, u32 offset);
void nvme_wait_status_ready(struct nvme_device *dev);

#include "assert.h"
#define CHECK_FATAL_STATUS(Device) assert(!(*nvme_reg(Device, NVME_CST) & 0x2))

#endif /* !NVME_H */
