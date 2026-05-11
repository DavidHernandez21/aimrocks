#!/bin/bash
set -euo pipefail

PREFIX=/opt/aimrocks_deps

curl -L https://github.com/google/snappy/archive/1.2.2.tar.gz -o snappy-1.2.2.tar.gz
tar zxvf snappy-1.2.2.tar.gz
cd snappy-1.2.2
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
	-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_C_FLAGS="-fPIC" \
	-DCMAKE_CXX_FLAGS="-fPIC" \
	-DSNAPPY_BUILD_TESTS=OFF \
	-DSNAPPY_BUILD_BENCHMARKS=OFF ..
cmake --build . -- -j"$(nproc)"
cmake --install .
cd ../..
rm -rf snappy-1.2.2 snappy-1.2.2.tar.gz
