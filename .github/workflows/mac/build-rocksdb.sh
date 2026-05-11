#!/bin/bash
set -e

cd $AIM_DEP_DIR
rm lib/*.dylib
curl -L https://github.com/facebook/rocksdb/archive/10.7.fb.tar.gz -o rocksdb-10.7.fb.tar.gz
tar zxvf rocksdb-10.7.fb.tar.gz
cd rocksdb-10.7.fb
LIBRARY_PATH=$AIM_DEP_DIR/lib PORTABLE=1 make shared_lib PLATFORM_SHARED_VERSIONED=false EXTRA_CXXFLAGS="-fPIC -I$AIM_DEP_DIR/include" USE_RTTI=0 DEBUG_LEVEL=0 -j12
strip -S librocksdb.dylib
install_name_tool -id @rpath/librocksdb.dylib librocksdb.dylib
cp librocksdb.dylib $AIM_DEP_DIR/lib
LIBRARY_PATH=$AIM_DEP_DIR/lib PREFIX="$AIM_DEP_DIR" PORTABLE=1 make install-headers PLATFORM_SHARED_VERSIONED=false EXTRA_CXXFLAGS="-fPIC -I$AIM_DEP_DIR/include" USE_RTTI=0 DEBUG_LEVEL=0 -j12
cd ..
rm -rf rocksdb-10.7.fb rocksdb-10.7.fb.tar.gz
