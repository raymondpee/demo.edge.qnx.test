#!/bin/bash

rm -rf FFmpeg
git clone -b n4.2.7 https://github.com/FFmpeg/FFmpeg &&\
cd FFmpeg
./configure --enable-cross-compile --cross-prefix=${QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0- --arch=aarch64le --disable-debug --enable-optimizations --enable-asm --disable-static --enable-shared --target-os=qnx --disable-ffplay --disable-ffprobe --prefix=`pwd`/target
make -j4