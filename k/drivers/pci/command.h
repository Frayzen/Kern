#ifndef COMMAND_H
#define COMMAND_H

#include "drivers/pci/pci.h"
void disable_io(struct pci_device* device);
void enable_io(struct pci_device* device);

#endif /* !COMMAND_H */
