#include "memalloc.h"
#include "k/types.h"
#include "panic.h"
#include "assert.h"
#include <stdio.h>
#include <string.h>

extern void *_end[]; /* kernel data end address */

static char *bitmap_start;
static void *mem_start;
static u32 nb_page = 0;

// Rounds a to the closest higher b multiple
static u32 round_mul(u32 a, u32 b)
{
	return (a + (b - (a % b)));
}

void init_memalloc(multiboot_info_t *info)
{
	u32 end_kern = round_mul((u32)_end, PAGE_SIZE);

	// mem_upper is in kb
	u32 nb_theorical_page = (info->mem_upper * 1024 - end_kern) / PAGE_SIZE;

	// Look for lowest value checking:
	// nb_allocate * 8 * PAGE_SIZE < nb_page - nb_allocate
	// 2 * nb_allocate < nb_page / (8 * PAGE_SIZE)
	u32 nb_bitmaps = 0;
	while (nb_bitmaps * 8 * PAGE_SIZE < nb_theorical_page - nb_bitmaps)
		nb_bitmaps++;
	printf("Can allocate %d pages (%d reserved for bitmap)\n",
	       (nb_theorical_page - nb_bitmaps), nb_bitmaps);
	nb_page = (nb_theorical_page - nb_bitmaps);
	bitmap_start = (void *)end_kern;
	mem_start = bitmap_start + PAGE_SIZE * nb_bitmaps;
	memset(bitmap_start, 0, PAGE_SIZE * nb_bitmaps);
}

void *mmap()
{
	for (u32 i = 0; i < nb_page; i++) {
		if (!(bitmap_start[i / 8] & (1 << (i % 8)))) {
			bitmap_start[i / 8] |= (1 << (i % 8));
			void *ret = mem_start + (PAGE_SIZE * i);
			memset(ret, 0, PAGE_SIZE);
			return ret;
		}
	}
	panic("No more avalailble pages");
	return NULL;
}

void munmap(void *ptr)
{
	assert((u32)ptr % PAGE_SIZE == 0);
	assert(ptr >= mem_start);
	u32 cur = (ptr - mem_start) / PAGE_SIZE;
	bitmap_start[cur / 8] &= ~(1 << (cur % 8));
}
