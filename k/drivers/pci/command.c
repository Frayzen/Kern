#include "command.h"
#include "drivers/pci/pci.h"
#include "k/types.h"

void disable_io(struct pci_device* device)
{
  u32 cur = pci_config_read(device->bus, device->slot, 0, 0x1);
  cur &= (~0x1);
  pci_config_write(device->bus, device->slot, 0, 0x1, cur);
}
void enable_io(struct pci_device* device)
{
  u32 cur = pci_config_read(device->bus, device->slot, 0, 0x1);
  cur |= 0x1;
  pci_config_write(device->bus, device->slot, 0, 0x1, cur);
}
