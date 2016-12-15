#!/bin/sh
set -e
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

. ./headers.sh

for PROJECT in $PROJECTS; do
  DESTDIR="$PWD/sysroot" $MAKE -C $PROJECT install
done
