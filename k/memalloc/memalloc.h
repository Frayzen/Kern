#ifndef MEMALLOC_H
#define MEMALLOC_H

#include "multiboot.h"

#define PAGE_SIZE 4096
#define BIPMAP_START

void init_memalloc(multiboot_info_t* info);

void *mmap();
void munmap(void *ptr);

#endif /* !MEMALLOC_H */
