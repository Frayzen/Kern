#!/bin/sh

make -j8
if [ $? -ne 0 ]; then
    echo "make failed"
    exit 1
fi
echo "CTRL+ALT+G to lose focus"
# echo c | bochs -q -rc /dev/stdin
bochs -q
# i3 split h >/dev/null 2>&1
# qemu-system-i386 -cdrom k.iso -serial stdio -s -S &
# sleep 0.5
# i3 split v >/dev/null 2>&1
# kitty gdb
