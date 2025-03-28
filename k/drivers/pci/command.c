#include "command.h"
#include "drivers/pci/pci.h"
#include "k/types.h"

void disable_io(struct pci_device* device)
{
  u32 cur = pcidev_readw(device, 0x1);
  cur &= (~0x1);
  pcidev_writew(device, 0x1, cur);
}
void enable_io(struct pci_device* device)
{
  u32 cur = pcidev_readw(device, 0x1);
  cur |= 0x1;
  pcidev_writew(device, 0x1, cur);
}
