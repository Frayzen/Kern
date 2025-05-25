# APIC (Advanced Programmable Interrupt Controller)

## Introdcution

The APIC is a system that helps your CPU manage hardware interrupts (signals from devices like keyboards, mice, network cards, etc.). It makes interrupt handling faster and more efficient, especially in multi-core systems.

In an APIC-based system, each CPU is made of a "core" and a "local APIC". The local APIC is responsible for handling cpu-specific interrupt configuration. Among other things, it contains the Local Vector Table (LVT) that translates events such as "internal clock" and other "local" interrupt sources into a interrupt vector

## Why do we need apic ?

**Old Way (PIC - 8259 chip):**
* Only supported 8-16 interrupts.
* All interrupts went to one CPU (bad for multi-core).
* Slow and limited.

**Modern Way (APIC):**
* Supports hundreds of interrupts.
* Can send interrupts to specific CPU cores.
* Faster and more flexible

## Configuration

The APIC system is divided into two primary components, each with distinct configuration roles:

### **1. I/O APIC (Global Interrupt Management)**

The I/O APIC is a system-wide interrupt controller that replaces the legacy 8259 PIC. It handles hardware interrupt routing from devices (e.g., keyboards, NICs, PCIe devices) to CPU cores. Unlike the PIC, which uses fixed IRQ numbers, the I/O APIC maps interrupts to programmable vectors (0–255) in the IDT.

### **2. Local APIC (Per-CPU Configuration)**

Each CPU core has its own Local APIC (LAPIC), configured via memory-mapped registers (address found in ACPI MADT).
The APIC timer is local and programmed via the LVT (Local Vector Table) timer entry. Unlike the PIT (IRQ 0), it operates independently per core, avoiding contention. The timer’s vector, mode (periodic/one-shot) and frequency can be configured in the local APIC registers.
The local APIC registers are also used to send IPIs to other cores. An Inter-Processor Interrupt (IPI) is a mechanism used in multi-core systems where one CPU core sends an interrupt directly to another core to coordinate tasks or synchronize data. Unlike hardware interrupts from devices, IPIs are generated and managed entirely by software. Common uses include scheduling, cache sync or debugging.
