#!/bin/sh
set -m
set -e

make -j8
if [ $? -ne 0 ]; then
    echo "make failed"
    exit 1
fi

# echo c | bochs -q -rc /dev/stdin
# bochs -q
# exit 0

which i3 2>/dev/null 1>&2
usei3=$?

echo "CTRL+ALT+G to lose focus"
echo "CTRL+ALT+2 to open qemu monitor"
if [ $usei3 ]; then
  i3 split h >/dev/null &
  wait $!
fi
kitty gdb ./k/k -ex "target remote localhost:1234" &
GDB_PID=$!
sleep 0.5
if [ $usei3 ]; then
  i3 split v >/dev/null &
  wait $!
fi
qemu-system-i386 -drive id=cdrom,if=ide,media=cdrom,readonly=on,file=k.iso \
  -drive file=disk.img,if=none,id=nvme0,format=raw \
  -device nvme,drive=nvme0,serial=1234,addr=0x5 \
  -trace enable="nvme_*",file=out \
  -serial stdio -s -S
wait
