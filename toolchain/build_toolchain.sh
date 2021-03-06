#!/bin/bash
# build_toolchain.sh Builds the cross compiler tool chain

#=========================================================
# Fetch all needed files
#=========================================================

# Get Binutils 2.32
wget -nc ftp://ftp.gnu.org/gnu/binutils/binutils-2.32.tar.gz

# Get GCC 8.3.0
wget -nc ftp://ftp.gnu.org/gnu/gcc/gcc-8.3.0/gcc-8.3.0.tar.gz

# Get Installation Requisites
sudo apt-get install -y texinfo

# Unpack source to src folder

mkdir -p src

for f in *.tar.*; do tar -xf $f -C src; done

#=========================================================
# Build Binutils
#=========================================================

CUR_DIR=$(pwd)

export PREFIX="$CUR_DIR/OS6_cross_cc"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"

chmod +x src/*

cd src

mkdir -p build-binutils
cd build-binutils

../binutils-2.32/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror

make -j8
make -j8 install

cd ..

#=========================================================
# Build GCC
#=========================================================

cd gcc-8.3.0 && contrib/download_prerequisites

cd ..

mkdir -p build-gcc
cd build-gcc

../gcc-8.3.0/configure \
    --target=$TARGET \
    --prefix="$PREFIX" \
    --disable-nls \
    --enable-languages=c,c++ \
    --disable-multilib \
    --disable-libstdc++-v3 \
    --disable-shared \
    --without-headers \
    --with-newlib

make -j8 all-gcc
make -j8 install-gcc

make -j8 all-target-libgcc
make -j8 install-target-libgcc

cd ../..

#=========================================================
# Final Cleanup
#=========================================================

rm -rf src
rm *.tar.*
