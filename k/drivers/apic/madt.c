#include "madt.h"
#include <stdio.h>

struct madt_entry *madt_find_next_entry(struct MADT *madt,
					struct madt_entry *cur_entry,
					enum madt_entry_type type)
{
	printf("APIC ADDR %x\n", madt->lapic_addr);
	u8 *cur_ptr = (u8 *)cur_entry;
	u8 *end_ptr = (u8 *)madt + madt->h.length;

	while (cur_ptr < end_ptr) { // Ensure we don't go past the MADT
		struct madt_entry *entry = (struct madt_entry *)cur_ptr;

		// Check if current entry matches the desired type
		if (entry->type == type) {
			return entry;
		}

		// Safety: Avoid infinite loop if entry->length is invalid
		if (entry->length == 0 || cur_ptr + entry->length > end_ptr) {
			break;
		}

		cur_ptr += entry->length; // Move to next entry
	}

	return NULL; // Not found
}
struct madt_entry *madt_find_entry(struct MADT *madt, enum madt_entry_type type)
{
	return madt_find_next_entry(madt, &madt->first_entry, type);
}
