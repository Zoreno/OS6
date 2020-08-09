#!/bin/bash

qemu-img create -f raw $1 $2

mkfs.ext2 -F $1

mkdir mnt && mkdir mnt/rootfs

sync

sudo mount -o loop disk.img mnt/rootfs

sudo cp -R ../root/* mnt/rootfs

sync

sudo umount mnt/rootfs

sudo rm -rf mnt