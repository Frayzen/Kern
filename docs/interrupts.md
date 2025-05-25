# Interrupts

When a hardware device (like a keyboard, network card, or timer) needs the CPU's attention, it sends an interrupt. The CPU stops what it's doing, handles the interrupt, and then resumes its previous task.

## IRQ (Interrupt Request)

Each hardware device is assigned an IRQ number (e.g., IRQ 1 = Keyboard, IRQ 14 = Primary ATA Hard Drive). The PIC (8259) or APIC manages these IRQs and forwards them to the CPU.

On x86 systems using the PIC (8259A), IRQ 0 is mapped to Interrupt 32 (0x20) in the IDT after PIC remapping.
Concerning APIC, Interrupts are assigned vectors (0-255) in the IDT.

## ISR (Interrupt Service Routine)

When an interrupt occurs, the CPU runs a special function called an ISR (Interrupt Service Routine). The ISR handles the interrupt (e.g., reads a keypress from the keyboard buffer).

##  IDT (Interrupt Descriptor Table)

## How It All Works Together
- You press a key → Keyboard sends IRQ 1.
- APIC/PIC forwards IRQ 1 to the CPU.
- CPU checks IDT for the ISR linked to IRQ 1.
- CPU jumps to keyboard_isr() (the ISR).
- ISR reads the key and stores it in a buffer.
- CPU returns to what it was doing before.
