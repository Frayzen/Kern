#include "rsdp.h"
#include "assert.h"
#include "drivers/rsdp/sdt.h"
#include "k/types.h"
#include <stdio.h>
#include <string.h>

static const char rsdp_sig[] = "RSD PTR ";
struct RSDP *find_rsdt(char *from, char *to)
{
	assert(from < to);
	char *ptr = from;
	while (ptr < to) {
		if (!strncmp(ptr, rsdp_sig, 8))
			return (void *)ptr;
		ptr += sizeof(u16);
	}
	return NULL;
}

void do_checksum(struct RSDP *rsdp)
{
	assert(!strncmp(rsdp->signature, rsdp_sig, 8));
	unsigned char sum = 0;
	u8 *ptr = (void *)rsdp;
	for (u32 i = 0; i < rsdp->length; i++) {
		sum += ((u8 *)ptr)[i];
	}
	assert(sum == 0);
}

struct RSDP *get_rsdp_address()
{
#define EBDA (char *)(0x00080000) // Extended BIOS Data Area
#define BIOS_AREA_START (char *)(0x000E0000)
#define BIOS_AREA_END (char *)0x000FFFFF

	static struct RSDP *ptr = NULL;
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
	static struct RSDP *rsdp = 0;
	if (rsdp == 0)
		rsdp = get_rsdp_address();
	int extended = rsdp->revision != 0;

	if (extended) {
		struct XSDT *xsdt = (void *)rsdp->xsdt_address;
		int entries = ((xsdt->h.length - sizeof(struct SDT_header)) /
			       sizeof(u64));
		printf("Entries is %d\n", entries);
		assert(entries < 100);
		for (int i = 0; i < entries; i++) {
			struct SDT_header *h =
				(struct SDT_header *)(xsdt->other_sdt[i]);
			if (!strncmp(h->signature, sig, 4))
				return (void *)h;
		}
	} else {
		struct RSDT *rsdt = (void *)rsdp->rsdt_address;
		int entries = ((rsdt->h.length - sizeof(struct SDT_header)) /
			       sizeof(u32));
		for (int i = 0; i < entries; i++) {
			struct SDT_header *h =
				(struct SDT_header *)(rsdt->other_sdt[i]);
			if (!strncmp(h->signature, sig, 4))
				return (void *)h;
		}
	}
	// No FACP found
	return NULL;
}
