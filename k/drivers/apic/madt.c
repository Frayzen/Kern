#include "madt.h"
#include <stdio.h>

static struct madt_entry *next_entry(struct madt *madt,
				     struct madt_entry *cur_entry,
				     enum madt_entry_type type)
{
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

struct madt_entry *madt_find_next_entry(struct madt *madt,
					struct madt_entry *cur_entry,
					enum madt_entry_type type)
{
	struct madt_entry *next =
		(struct madt_entry *)((u8 *)cur_entry + cur_entry->length);
	return next_entry(madt, next, type);
}
struct madt_entry *madt_find_entry(struct madt *madt, enum madt_entry_type type)
{
	return next_entry(madt, &madt->first_entry, type);
}
