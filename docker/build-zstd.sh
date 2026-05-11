#!/bin/bash
curl -L https://github.com/facebook/zstd/archive/v1.5.7.tar.gz -o zstd-1.5.7.tar.gz
tar zxvf zstd-1.5.7.tar.gz
cd zstd-1.5.7
make CFLAGS="-fPIC" CXXFLAGS="-fPIC"
make install PREFIX=$PWD/..
cd ..
rm -rf zstd-1.5.7 zstd-1.5.7.tar.gz