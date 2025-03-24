/*
 * Copyright (c) LSE
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY LSE AS IS AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL LSE BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "drivers/nvme/nvme.h"
#include "drivers/pci/pci.h"
#include "fs/fs.h"
#include "gdt/gdt.h"
#include "interrupts/ints.h"
#include "interrupts/keyboard.h"
#include "memalloc/memalloc.h"
#include "memory.h"
#include "panic.h"
#include "serial.h"
#include "stdio.h"
#include <k/kstd.h>

#include "multiboot.h"

void test_file(void)
{
	int fd = open("/boot/grub/grub.cfg");
	char buf[2048];
	// Read 10 bytes
	int len = read(fd, buf, 10);
	printf("Read %d bytes\n", len, buf);
	printf("%s\n", buf);

	// Read 10 bytes
	len = read(fd, buf, 10);
	printf("Read %d bytes\n", len, buf);
	printf("%s\n", buf);

	// Read 10 bytes from the end
	if (seek(fd, -10, SEEK_END) == -1)
		panic("Seek failed\n");
	len = read(fd, buf, 1024);
	printf("Read %d bytes\n", len, buf);
	printf("%s\n", buf);

	close(fd);
}

void k_main(unsigned long magic, multiboot_info_t *info)
{
	(void)magic;
	(void)info;

	char star[4] = "|/-\\";
	char *fb = (void *)0xb8000;

	asm volatile("cli" :);
	setup_gdt();
	setup_idt();
	asm volatile("sti" :);

  memory_init(info);
  init_memalloc(info);

	setup_fs();
	/* test_file(); */

  init_nvme();

	for (unsigned i = 0;;) {
		int c = get_last_key();
		if (c != -1)
			printf("KB %d\n", c);
		*fb = star[i++ % 4];
	}

	for (;;)
		asm volatile("hlt");
}
