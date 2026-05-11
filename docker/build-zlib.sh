#!/bin/bash
curl -L https://www.zlib.net/fossils/zlib-1.3.1.tar.gz -o zlib-1.3.1.tar.gz
tar zxvf zlib-1.3.1.tar.gz
cd zlib-1.3.1/
ls -lhatr
./configure
make CFLAGS="-fPIC" CXXFLAGS="-fPIC"
make install prefix=..
cd ..
rm -rf zlib-1.3.1/ zlib-1.3.1.tar.gz
