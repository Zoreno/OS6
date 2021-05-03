#!/bin/bash -e

qemu-img create -f raw $1 $2

mkfs.ext2 -F $1 -d ../root

