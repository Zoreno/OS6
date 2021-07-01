#!/bin/bash
# build_toolchain.sh Builds the cross compiler tool chain

#=========================================================
# Fetch all needed files
#=========================================================

BINUTILS_VERSION=2.35
GCC_VERSION=10.2.0

# Get Binutils 2.32
wget -nc ftp://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz

# Get GCC 8.3.0
wget -nc ftp://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz

# Get Installation Requisites
export DEBIAN_FRONTEND="noninteractive"
apt-get install -y texinfo

# Unpack source to src folder

mkdir -p src

for f in *.tar.*; do tar -xf $f -C src; done

#=========================================================
# Build Binutils
#=========================================================

CUR_DIR=$(pwd)

export PREFIX="/opt/OS6_cross_cc"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"

chmod +x src/*

cd src

mkdir -p build-binutils
cd build-binutils

../binutils-$BINUTILS_VERSION/configure \
    --target=$TARGET \
    --prefix="$PREFIX" \
    --with-sysroot \
    --disable-nls \
    --disable-werror

make -j4
make -j4 install

cd ..

#=========================================================
# Build GCC
#=========================================================

cd gcc-$GCC_VERSION && contrib/download_prerequisites

cd ..

mkdir -p build-gcc
cd build-gcc

../gcc-$GCC_VERSION/configure \
    --target=$TARGET \
    --prefix="$PREFIX" \
    --disable-nls \
    --enable-languages=c,c++ \
    --disable-multilib \
    --disable-libstdc++-v3 \
    --disable-shared \
    --without-headers \
    --with-newlib \
    --with-sysroot

make -j4 all-gcc
make -j4 install-gcc

make -j4 all-target-libgcc
make -j4 install-target-libgcc

cd ../..

#=========================================================
# Final Cleanup
#=========================================================

rm -rf src
rm *.tar.*
