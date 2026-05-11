#!/bin/bash
set -euo pipefail

PREFIX=/opt/aimrocks_deps

curl -L https://github.com/lz4/lz4/archive/v1.10.0.tar.gz -o lz4-1.10.0.tar.gz
tar zxvf lz4-1.10.0.tar.gz
cd lz4-1.10.0
make -j"$(nproc)" CFLAGS="-fPIC" CXXFLAGS="-fPIC"
make PREFIX="${PREFIX}" install
cd ..
rm -rf lz4-1.10.0 lz4-1.10.0.tar.gz
