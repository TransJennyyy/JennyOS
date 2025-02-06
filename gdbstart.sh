gdb -ex "symbol-file Kernel.debug" -ex "target remote localhost:1234"

pkill qemu
