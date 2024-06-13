#!/bin/sh
set -m

make -j8
if [ $? -ne 0 ]; then
    echo "make failed"
    exit 1
fi
echo "CTRL+ALT+G to lose focus"
# echo c | bochs -q -rc /dev/stdin
# bochs -q
# exit 0

i3 split h >/dev/null &
wait $!
kitty gdb ./k/k &
sleep 0.5
i3 split v >/dev/null &
wait $!
qemu-system-i386 -cdrom k.iso -serial stdio -s -S
wait
