#!/bin/sh
set -e
set -m

show_help(){
  echo "Usage: launch.sh <compile|run|debug|help>"
}

compile() {
  make -j8 $@
}

run() {
  echo "[INFO] CTRL+ALT+G to lose focus (by default)"
  if $1; then
    $TERMINAL gdb ./k/k &
    sleep 0.5
    qemu-system-i386 -drive id=cdrom,if=ide,media=cdrom,readonly=on,file=k.iso -serial stdio -s -S
    wait
  else
    qemu-system-x86_64 -cdrom k.iso
  fi
}


if [ $# -ne 1 ]; then
  show_help
  exit 1
else
  case $1 in
    help)
      show_help
      ;;
    compile)
      compile
      ;;
    run)
      run false
      ;;
    debug)
      run true
      ;;

  esac;
fi
