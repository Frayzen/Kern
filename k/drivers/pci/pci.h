#ifndef PCI_H
#define PCI_H


/*
Offset |Bits 31-24 | Bits 23-16  | Bits 15-8    | Bits 7-0 
=============================================================
0x0    |      Device ID          |         Vendor ID
0x4 	 |      Status             |          Command
0x8 	 | Class code | Subclass 	 | Prog IF 	    | Revision ID
0xC 	 |    BIST 	  | Header Type| Latency Timer| Cache Line Size

/!\ The lower addresses contain the least significant portions of the field
*/
#include "k/compiler.h"
#include "k/types.h"
#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

struct pci_device {
  u8 bus;
  u8 slot;
  u16 deviceId;
  u16 vendorId;
  u16 status;
  // might add command
  u8 classCode;
  u8 subClass;
  // ... 
  u8 headerType;
} __packed;

#define PCI_BAR0 0x10
#define PCI_BAR1 0x14

u32 pci_config_read(u8 bus, u8 slot, u8 func, u8 offset);
u16 pci_config_read_upper(u8 bus, u8 slot, u8 func, u8 offset);
u16 pci_config_read_lower(u8 bus, u8 slot, u8 func, u8 offset);

// returns non zero if found, updates the out value accordingly
int look_for_device(u8 classCode, u8 subClass, struct pci_device* out);

#endif /* !PCI_H */
