#!/bin/sh
make -j8
qemu-system-x86_64 -cdrom k.iso -serial stdio
