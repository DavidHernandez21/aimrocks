#pragma once

#include "rocksdb/options.h"
#include "rocksdb/compression_type.h"

namespace py_rocks {

// =============================================================================
// DEPRECATED OPTION COMPATIBILITY LAYER
// =============================================================================
// These functions handle options that were removed in RocksDB 6.x-7.x but
// may still be referenced by legacy code (like Aim). They now properly
// document their behavior rather than pretending to work via broken macros.
// =============================================================================

// hash_index_allow_collision: Removed in RocksDB 6.6 (2019)
// This option controlled whether hash index would allow collisions.
// Modern RocksDB uses optimized index formats that handle this automatically.
// For backward compatibility: This is now a no-op that accepts but ignores the value.
inline void set_hash_index_allow_collision(rocksdb::BlockBasedTableOptions& opts, bool v) {
    // No-op: Option removed in RocksDB 6.6+
    // Modern index formats handle collisions efficiently by design
    (void)opts; 
    (void)v;
}

// block_cache_compressed: Removed in RocksDB 6.29 (2021)  
// This was a separate cache for compressed blocks. Modern RocksDB uses
// a unified cache system with compression-aware eviction policies.
// For backward compatibility: This is now a no-op that accepts but ignores the cache.
inline void set_block_cache_compressed(rocksdb::BlockBasedTableOptions& opts, 
                                       std::shared_ptr<rocksdb::Cache> cache) {
    // No-op: Option removed in RocksDB 6.29+
    // Use block_cache with cache_index_and_filter_blocks instead
    (void)opts; 
    (void)cache;
}

// max_mem_compaction_level: Removed in RocksDB 2.x (2014)
// This controlled which level memtables would compact to.
// Modern RocksDB uses max_compaction_bytes and other options for this.
// For backward compatibility: Getter returns 0, setter is a no-op.
inline void set_max_mem_compaction_level(rocksdb::ColumnFamilyOptions& cops, int v) {
    // No-op: Option removed in RocksDB 2.x (2014)
    // Modern alternative: Use max_compaction_bytes and level0_file_num_compaction_trigger
    (void)cops; 
    (void)v;
}

inline int get_max_mem_compaction_level(const rocksdb::ColumnFamilyOptions& cops) {
    // Always returns 0 - option removed in RocksDB 2.x (2014)
    (void)cops;
    return 0;
}

} // namespace py_rocks
