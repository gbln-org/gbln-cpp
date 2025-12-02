// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

#include "gbln/serialiser.hpp"
#include "gbln/detail/ffi.hpp"
#include "gbln/detail/managed_value.hpp"
#include "gbln/detail/conversion.hpp"

namespace gbln {

std::string to_string(const Value& value, bool mini) {
    // Convert C++ value to FFI
    GblnValue* ffi_value = detail::cpp_to_ffi(value);
    
    // Wrap for automatic cleanup
    detail::ManagedValue managed(ffi_value);
    
    // Serialise
    char* result_str = mini ? 
        gbln_to_string(managed.get()) : 
        gbln_to_string_pretty(managed.get());
    
    if (!result_str) {
        throw SerialiseError("Serialisation returned null pointer");
    }
    
    // Wrap string for automatic cleanup
    detail::ManagedString managed_str(result_str);
    
    // Return as std::string (copy)
    return managed_str.str();
}

std::string to_string_pretty(const Value& value, int indent) {
    // Pretty format ignores indent parameter in current FFI
    // (FFI uses fixed formatting)
    (void)indent;  // Suppress unused parameter warning
    
    return to_string(value, false);
}

} // namespace gbln
