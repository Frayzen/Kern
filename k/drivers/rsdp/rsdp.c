#include "rsdp.h"
#include "assert.h"
#include "drivers/rsdp/sdt.h"
#include "k/types.h"
#include <stdio.h>
#include <string.h>

static const char rsdp_sig[] = "RSD PTR ";
// returns 1 if signature found, 0 otherwise
struct RSDT *find_rsdt(char *from, char *to)
{
	assert(from < to);
	char *ptr = from;
	int cur = 0;
	while (ptr < to) {
		if (*ptr == rsdp_sig[cur])
			cur++;
		else
			cur = 0;
		ptr++;
		if (cur == sizeof(rsdp_sig))
			return (struct RSDT *)(ptr - cur);
	}
	return NULL;
}

void do_checksum(struct RSDT *rsdp)
{
	assert(!strncmp(rsdp->h.signature, rsdp_sig, 8));
	unsigned char sum = 0;
	u8 *ptr = (void *)rsdp;

	for (u32 i = 0; i < rsdp->h.length; i++) {
		sum += ((u8 *)ptr)[i];
	}

	assert(sum == 0);
}

struct RSDT *get_rsdt_address()
{
#define EBDA (char *)(0x00080000) // Extended BIOS Data Area
#define BIOS_AREA_START (char *)(0x000E0000)
#define BIOS_AREA_END (char *)0x000FFFFF

	static struct RSDT *ptr = NULL;
	if (ptr != NULL)
		return (void *)ptr;
	ptr = find_rsdt(EBDA, EBDA + 1024);
	if (ptr == NULL)
		ptr = find_rsdt(BIOS_AREA_START, BIOS_AREA_END);
	assert(ptr != NULL);
	do_checksum(ptr);
	return (void *)ptr;
}

void *find_SDT(char sig[4])
{
	struct RSDT *rsdt = get_rsdt_address();
	int extended = rsdt->h.revision != 0;
	int entries = (rsdt->h.length - sizeof(struct RSDT_header)) /
		      (extended ? sizeof(u64) : sizeof(u32));

	for (int i = 0; i < entries; i++) {
		struct SDT_header *h =
			(struct SDT_header *)(rsdt->other_sdt[i]);
		if (!strncmp(h->signature, sig, 4))
			return (void *)h;
	}

	// No FACP found
	return NULL;
}
