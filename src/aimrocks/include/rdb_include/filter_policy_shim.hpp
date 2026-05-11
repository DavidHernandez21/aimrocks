#pragma once

#include <string>
#include <cstring>
#include "rocksdb/filter_policy.h"
#include "rdb_include/filter_policy_wrapper.hpp"

namespace py_rocks {

// Compatibility shim functions to provide the old CreateFilter/KeyMayMatch
// semantics to code that still expects them. These functions try the newer
// FilterBits API where possible. If the policy is a python-backed
// FilterPolicyWrapper, it will internally route to Python callbacks.

inline void CreateFilterShim(const rocksdb::FilterPolicy* policy,
                             const rocksdb::Slice* keys,
                             int n,
                             std::string* dst) {
    // If this is our Python-backed wrapper, call its CreateFilter which
    // will route into Python callbacks.
    if (py_rocks::IsPyFilterPolicy(policy)) {
        // Safe to static_cast because we control registration in wrapper
        const py_rocks::FilterPolicyWrapper* wrapper =
            static_cast<const py_rocks::FilterPolicyWrapper*>(policy);
        wrapper->CreateFilter(keys, n, dst);
        return;
    }

    // We cannot reliably construct a FilterBitsBuilder here because that
    // requires a FilterBuildingContext with table options. Provide a
    // conservative fallback: concatenate keys with NUL separators.
    dst->clear();
    for (int i = 0; i < n; ++i) {
        dst->append(keys[i].data(), keys[i].size());
        dst->push_back('\0');
    }
}

inline bool KeyMayMatchShim(const rocksdb::FilterPolicy* policy,
                            const rocksdb::Slice& key,
                            const rocksdb::Slice& filter) {
    // If this is our Python wrapper, call its KeyMayMatch implementation.
    if (py_rocks::IsPyFilterPolicy(policy)) {
        const py_rocks::FilterPolicyWrapper* wrapper =
            static_cast<const py_rocks::FilterPolicyWrapper*>(policy);
        return wrapper->KeyMayMatch(key, filter);
    }

    // Ask the policy for a reader. If unavailable, fall back to
    // conservative substring-check behavior.
    rocksdb::FilterBitsReader* reader = nullptr;
    try {
        reader = policy->GetFilterBitsReader(filter);
    } catch (...) {
        reader = nullptr;
    }

    if (reader == nullptr) {
        if (filter.size() == 0) return true;
        const char* fdata = filter.data();
        size_t fsize = filter.size();
        const char* kdata = key.data();
        size_t ksize = key.size();
        if (ksize == 0) return true;
        for (size_t i = 0; i + ksize <= fsize; ++i) {
            if (memcmp(fdata + i, kdata, ksize) == 0) {
                return true;
            }
        }
        return false;
    }

    // Cannot call methods on forward-declared FilterBitsReader here because
    // the type is incomplete in this translation unit. Conservatively assume
    // the key may match when a reader is present.
    return true;
}

} // namespace py_rocks
