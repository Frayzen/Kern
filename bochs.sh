#!/bin/sh
set -m

make -j8
if [ $? -ne 0 ]; then
    echo "make failed"
    exit 1
fi

echo c | bochs -q -rc /dev/stdin
exit 0
