# MSI (Message Signaled Interrupts)

Message Signaled Interrupts (MSI) is a modern interrupt mechanism used primarily by PCIe devices to notify the CPU of events without relying on traditional dedicated interrupt pins (IRQs). Instead of using physical wires to signal interrupts, MSI allows devices to send interrupts by writing a small message directly to a specific memory address configured by the operating system. This memory write is detected by the CPU's Advanced Programmable Interrupt Controller (APIC), which then triggers the corresponding interrupt handler.


