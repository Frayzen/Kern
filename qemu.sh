#!/bin/sh
set -m

make -j8
if [ $? -ne 0 ]; then
    echo "make failed"
    exit 1
fi

# echo c | bochs -q -rc /dev/stdin
# bochs -q
# exit 0

echo "CTRL+ALT+G to lose focus"
i3 split h >/dev/null &
wait $!
kitty gdb ./k/k &
sleep 0.5
i3 split v >/dev/null &
wait $!
qemu-system-i386 -drive id=cdrom,if=ide,media=cdrom,readonly=on,file=k.iso -enable-kvm -serial stdio -s -S
wait
