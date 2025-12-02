// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file managed_value.hpp
 * @brief RAII wrappers for C FFI resources
 * 
 * Provides automatic memory management for C FFI pointers using RAII.
 */

#pragma once

#include "ffi.hpp"
#include "../exceptions.hpp"
#include <memory>
#include <stdexcept>

namespace gbln::detail {

/// RAII wrapper for GblnValue with automatic cleanup.
/// Non-copyable, movable for efficient resource management.
class ManagedValue {
public:
    /// Create from raw pointer (takes ownership)
    /// @throws std::invalid_argument if pointer is null
    explicit ManagedValue(GblnValue* ptr) 
        : ptr_(ptr, [](GblnValue* p) {
            if (p) gbln_value_free(p);
        })
    {
        if (!ptr) {
            throw std::invalid_argument("Cannot create ManagedValue from null pointer");
        }
    }
    
    // Non-copyable (unique ownership)
    ManagedValue(const ManagedValue&) = delete;
    ManagedValue& operator=(const ManagedValue&) = delete;
    
    // Movable (transfer ownership)
    ManagedValue(ManagedValue&&) = default;
    ManagedValue& operator=(ManagedValue&&) = default;
    
    /// Get raw pointer
    GblnValue* get() const { return ptr_.get(); }
    
    /// Arrow operator for convenience
    GblnValue* operator->() const { return ptr_.get(); }
    
    /// Bool conversion for if (managed_value) checks
    explicit operator bool() const { return ptr_ != nullptr; }
    
private:
    std::unique_ptr<GblnValue, void(*)(GblnValue*)> ptr_;
};

/// RAII wrapper for C strings from GBLN.
/// Automatically calls gbln_string_free on destruction.
class ManagedString {
public:
    /// Create from raw string pointer (takes ownership)
    /// @throws std::invalid_argument if pointer is null
    explicit ManagedString(char* ptr)
        : ptr_(ptr, [](char* p) {
            if (p) gbln_string_free(p);
        })
    {
        if (!ptr) {
            throw std::invalid_argument("Cannot create ManagedString from null pointer");
        }
    }
    
    // Non-copyable
    ManagedString(const ManagedString&) = delete;
    ManagedString& operator=(const ManagedString&) = delete;
    
    // Movable
    ManagedString(ManagedString&&) = default;
    ManagedString& operator=(ManagedString&&) = default;
    
    /// Get C string
    const char* get() const { return ptr_.get(); }
    
    /// Implicit conversion to const char*
    operator const char*() const { return ptr_.get(); }
    
    /// Get as std::string
    std::string str() const { return std::string(ptr_.get()); }
    
private:
    std::unique_ptr<char, void(*)(char*)> ptr_;
};

/// RAII wrapper for GblnConfig.
/// Automatically calls gbln_config_free on destruction.
class ManagedConfig {
public:
    /// Create from raw pointer (takes ownership)
    /// @throws std::invalid_argument if pointer is null
    explicit ManagedConfig(GblnConfig* ptr)
        : ptr_(ptr, [](GblnConfig* p) {
            if (p) gbln_config_free(p);
        })
    {
        if (!ptr) {
            throw std::invalid_argument("Cannot create ManagedConfig from null pointer");
        }
    }
    
    // Non-copyable
    ManagedConfig(const ManagedConfig&) = delete;
    ManagedConfig& operator=(const ManagedConfig&) = delete;
    
    // Movable
    ManagedConfig(ManagedConfig&&) = default;
    ManagedConfig& operator=(ManagedConfig&&) = default;
    
    /// Get raw pointer
    GblnConfig* get() const { return ptr_.get(); }
    
    /// Arrow operator
    GblnConfig* operator->() const { return ptr_.get(); }
    
private:
    std::unique_ptr<GblnConfig, void(*)(GblnConfig*)> ptr_;
};

/// RAII wrapper for object keys array.
/// Automatically calls gbln_keys_free on destruction.
class ManagedKeys {
public:
    /// Create from keys array and count
    ManagedKeys(char** keys, std::size_t count)
        : keys_(keys), count_(count)
    {
        if (!keys && count > 0) {
            throw std::invalid_argument("Null keys with non-zero count");
        }
    }
    
    // Non-copyable
    ManagedKeys(const ManagedKeys&) = delete;
    ManagedKeys& operator=(const ManagedKeys&) = delete;
    
    // Movable
    ManagedKeys(ManagedKeys&& other) noexcept
        : keys_(other.keys_), count_(other.count_)
    {
        other.keys_ = nullptr;
        other.count_ = 0;
    }
    
    ManagedKeys& operator=(ManagedKeys&& other) noexcept {
        if (this != &other) {
            free_keys();
            keys_ = other.keys_;
            count_ = other.count_;
            other.keys_ = nullptr;
            other.count_ = 0;
        }
        return *this;
    }
    
    /// Destructor - free keys
    ~ManagedKeys() {
        free_keys();
    }
    
    /// Get key at index
    /// @throws std::out_of_range if index >= count
    const char* operator[](std::size_t index) const {
        if (index >= count_) {
            throw std::out_of_range("Key index out of range");
        }
        return keys_[index];
    }
    
    /// Get number of keys
    std::size_t size() const { return count_; }
    
    /// Check if empty
    bool empty() const { return count_ == 0; }
    
private:
    void free_keys() {
        if (keys_) {
            gbln_keys_free(keys_, count_);
            keys_ = nullptr;
            count_ = 0;
        }
    }
    
    char** keys_;
    std::size_t count_;
};

} // namespace gbln::detail
