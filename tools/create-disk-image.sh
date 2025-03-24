#!/bin/sh

set -e

disk_filename=$1
root_directory=$1

if [ -z "$disk_filename" ]; then
  echo "Please provide a disk filename"
  exit 1;
fi

if [ -z "$disk_filename" ]; then
  echo "Please provide a root directory"
  exit 1;
fi

dd if=/dev/null of=$disk_filename bs=1M seek=10240
mkfs.ext2 -F "$disk_filename" -d "$root_directory"
