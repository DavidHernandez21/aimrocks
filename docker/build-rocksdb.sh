#!/bin/bash
set -euo pipefail

PREFIX=/opt/aimrocks_deps
LIBDIR="${PREFIX}/lib"
INCLUDEDIR="${PREFIX}/include"

# check OS version
if [[ -f /etc/redhat-release ]]
then
  if [[ $(< /etc/redhat-release) == "CentOS release 5"* ]]
  then
    # CentOS 5
    export platform=centos_5
  fi
fi

# Clean only prior RocksDB shared objects, keep dependency libs intact.
rm -f "${LIBDIR}/librocksdb.so" "${LIBDIR}/librocksdb.so."*

curl -L https://github.com/facebook/rocksdb/archive/10.7.fb.tar.gz -o rocksdb-10.7.fb.tar.gz
tar zxvf rocksdb-10.7.fb.tar.gz
cd rocksdb-10.7.fb
if [[ ${platform:-} == centos_5 ]]
then
  cp /opt/aimrocks_deps/rocksdb_sched.patch .
  patch port/port_posix.cc rocksdb_sched.patch
fi

JOBS=$(nproc)
LIBRARY_PATH="${LIBDIR}" \
PORTABLE=1 \
make shared_lib PLATFORM_SHARED_VERSIONED=false \
  EXTRA_CXXFLAGS="-fPIC -I${INCLUDEDIR} -std=c++20" \
  EXTRA_CFLAGS="-fPIC" \
  USE_RTTI=0 DEBUG_LEVEL=0 -j"${JOBS}"

strip --strip-debug librocksdb.so
cp librocksdb.so "${LIBDIR}/"
PORTABLE=1 make PREFIX="${PREFIX}" DEBUG_LEVEL=0 install-headers
cd ..
rm -rf rocksdb-10.7.fb rocksdb-10.7.fb.tar.gz
