# !/bin/bash

./qemu-8.2.0/prefix/bin/qemu-system-x86_64 \
-m 1G \
-nographic \
-nic user,hostfwd=tcp::1022-:22 \
-hda alpine.qcow2 \
-kernel linux-6.6/arch/x86_64/boot/bzImage \
-enable-kvm \
-device my_rng \
-append "console=ttyS0 root=/dev/sda3"