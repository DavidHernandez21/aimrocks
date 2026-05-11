#pragma once

#include "rocksdb/filter_policy.h"
#include "rocksdb/env.h"
#include <stdexcept>
#include <mutex>
#include <unordered_set>

using std::string;
using rocksdb::FilterPolicy;
using rocksdb::Slice;
using rocksdb::Logger;
using rocksdb::FilterBitsBuilder;
using rocksdb::FilterBitsReader;
using rocksdb::FilterBuildingContext;

namespace py_rocks {

// Registry of python-backed FilterPolicy pointers. We cannot rely on RTTI
// (builds use -fno-rtti), so wrappers register themselves here and the
// shim can detect them safely.
static std::unordered_set<const FilterPolicy*> g_py_filter_policies;
static std::mutex g_py_filter_policies_mutex;

inline void register_py_filter_policy(const FilterPolicy* p) {
    std::lock_guard<std::mutex> lk(g_py_filter_policies_mutex);
    g_py_filter_policies.insert(p);
}

inline void unregister_py_filter_policy(const FilterPolicy* p) {
    std::lock_guard<std::mutex> lk(g_py_filter_policies_mutex);
    g_py_filter_policies.erase(p);
}

inline bool IsPyFilterPolicy(const FilterPolicy* p) {
    std::lock_guard<std::mutex> lk(g_py_filter_policies_mutex);
    return g_py_filter_policies.find(p) != g_py_filter_policies.end();
}

    class FilterPolicyWrapper: public FilterPolicy {
        public:
            typedef void (*create_filter_func)(
                void* ctx,
                Logger*,
                string&,
                const Slice* keys,
                int n,
                string* dst);

            typedef bool (*key_may_match_func)(
                void* ctx,
                Logger*,
                string&,
                const Slice& key,
                const Slice& filter);

            FilterPolicyWrapper(
                string name,
                void* ctx,
                create_filter_func create_filter_callback,
                key_may_match_func key_may_match_callback):
                    name(name),
                    ctx(ctx),
                    create_filter_callback(create_filter_callback),
                    key_may_match_callback(key_may_match_callback)
            {}

            virtual ~FilterPolicyWrapper() {
                unregister_py_filter_policy(this);
            }

            virtual void
            CreateFilter(const Slice* keys, int n, std::string* dst) const {
                string error_msg;

                this->create_filter_callback(
                    this->ctx,
                    this->info_log.get(),
                    error_msg,
                    keys,
                    n,
                    dst);

                if (error_msg.size()) {
                    throw std::runtime_error(error_msg.c_str());
                }
            }

            virtual bool
            KeyMayMatch(const Slice& key, const Slice& filter) const {
                string error_msg;
                bool val;

                val = this->key_may_match_callback(
                    this->ctx,
                    this->info_log.get(),
                    error_msg,
                    key,
                    filter);

                if (error_msg.size()) {
                    throw std::runtime_error(error_msg.c_str());
                }
                return val;
            }

            virtual const char* Name() const {
                return this->name.c_str();
            }

            // New API in newer RocksDB versions: provide a compatibility name.
            // Default to the policy Name to preserve backward compatibility.
            virtual const char* CompatibilityName() const override {
                return this->Name();
            }

            // New API: return a FilterBitsBuilder based on the provided context.
            // We don't support custom FilterBitsBuilder from Python callbacks yet,
            // so return nullptr to indicate "no custom builder" and let RocksDB
            // fall back to built-in policies.
            virtual FilterBitsBuilder* GetBuilderWithContext(
                const rocksdb::FilterBuildingContext& /*context*/) const override {
                return nullptr;
            }

            // New API: return a FilterBitsReader for the given contents. We do not
            // provide a reader from Python callbacks, so return nullptr to let
            // RocksDB handle built-in readers.
            virtual FilterBitsReader* GetFilterBitsReader(
                const rocksdb::Slice& /*contents*/) const override {
                return nullptr;
            }

            void set_info_log(std::shared_ptr<Logger> info_log) {
                this->info_log = info_log;
            }

            // Register this instance for shim detection. Call here so the
            // shim can avoid dynamic_cast and RTTI.
            void register_self() {
                register_py_filter_policy(this);
            }

        private:
            string name;
            void* ctx;
            create_filter_func create_filter_callback;
            key_may_match_func key_may_match_callback;
            std::shared_ptr<Logger> info_log;
    };
}
