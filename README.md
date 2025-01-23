# README
Goulven Le Pennec<br/>
M2 ASSEL<br/>
ENSTA

## 1 - Introduction
In this git will be found the main modified files used to setup a kernel on linux.
Two directories have been created:
- One dedicated to Qemu 8.2
- One dedicated to Linux 6.6

Outside there is:
- the virtual machine (VM) launcher: `launch-vm.sh`
- **main.c**: a file to copy and compile in the VM to run the random number generator

## 2 - Qemu files (qemu directory)
- **my-rng.c**: contains the functions that implement the random number generator function found in `qemu/hw/misc/`.
- **meson.build**: contains the updated version of the system, considering the *my-rng.c* file at the same location.

## 3 - Linux files (linux directory)
- **main.c**: this file basicly is the entry point of the kernel. It is found in the `linux-6.6/init` directory. This version contains a series of tests on the virtual device based on assessing if the random number generator function works properly.
- **my-rng.c**: the file is located in `linux-6.6/drivers/misc` and contains the reimplemented functions that read the random number and write the seed.
- **Makefile**: still in the `linux-6.6/drivers/misc` directory this file has been updated so it considers the *my-rng.c* file.


## 4 - More steps:
Let's automate the creation of the virtual file that will be used to have some user space to connect to the device:
in `/etc/init.d`, create `init-my-rng-virtual-file` and fill it with:
```bash
#!/sbin/openrc-run

mknod /dev/my_rng_driver c 250 0
```
Set this file to executable (`chmod +x /etc/init.d/init-my-rng-virtual-file`)

