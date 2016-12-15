#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp ./kernel/argon.bin  ./isodir/boot
cp ./boot/grub/stage2_eltorito  ./isodir/boot/grub/stage2_eltorito
cp ./boot/grub/menu.lst  ./isodir/boot/grub/menu.lst

genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o argon.iso isodir


