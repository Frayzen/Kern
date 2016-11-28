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
#include <k/kstd.h>
#include <stdio.h>

#include "console.h"
#include "init.h"
#include "kfs.h"
#include "multiboot.h"
#include "syscall.h"

void k_main(unsigned long magic, multiboot_info_t *info)
{
	(void)magic;

	console_init();

	char *cmdline = (char *)info->cmdline;

	printf("[+] cmdline: %s\n", cmdline);

	init_pm();
	init_memory();
	init_interrupt();
	init_pic();
	init_kbd();
	init_pit();
	init_sound();
	init_syscall();
	write_init();
	init_vga();

	if (kfs_init((void *)((module_t *)info->mods_addr)[0].mod_start) < 0)
		printf("[+] unable to init kfs\n");

	if (init_rom(cmdline) < 0)
		printf("[+] unable to init rom \"%s\"\n", cmdline);

	exec_rom();

	while (1)
		asm volatile ("hlt\n\t");
}
