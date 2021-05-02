#!/bin/bash -e

qemu-img create -f raw $1 $2

mkfs.ext2 -F $1

mkdir -p mnt && mkdir -p mnt/rootfs

sync

sudo mount -o loop $1 mnt/rootfs
sudo cp -R ../root/* mnt/rootfs

sync

sudo umount mnt/rootfs
sudo rm -rf mnt
