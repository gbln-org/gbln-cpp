// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

#include "gbln/detail/conversion.hpp"
#include <limits>
#include <cstring>

namespace gbln::detail {

/* ========================================================================== */
/* FFI → C++ Conversion Implementation                                        */
/* ========================================================================== */

Value ffi_to_cpp(const GblnValue* ffi_value) {
    if (!ffi_value) {
        throw ParseError("Null FFI value pointer");
    }
    
    // Get type
    GblnValueType type = gbln_value_type(ffi_value);
    
    // Handle each type
    switch (type) {
        case Null: {
            return Value(nullptr);
        }
        
        case Bool: {
            bool ok = false;
            bool value = gbln_value_as_bool(ffi_value, &ok);
            if (!ok) {
                throw ParseError("Failed to extract boolean value");
            }
            return Value(value);
        }
        
        case I8:
        case I16:
        case I32:
        case I64:
        case U8:
        case U16:
        case U32:
        case U64: {
            // Extract as i64 (all integers fit)
            bool ok = false;
            std::int64_t value = gbln_value_as_i64(ffi_value, &ok);
            if (!ok) {
                throw ParseError("Failed to extract integer value");
            }
            return Value(value);
        }
        
        case F32: {
            bool ok = false;
            float value = gbln_value_as_f32(ffi_value, &ok);
            if (!ok) {
                throw ParseError("Failed to extract f32 value");
            }
            return Value(static_cast<double>(value));
        }
        
        case F64: {
            bool ok = false;
            double value = gbln_value_as_f64(ffi_value, &ok);
            if (!ok) {
                throw ParseError("Failed to extract f64 value");
            }
            return Value(value);
        }
        
        case Str: {
            bool ok = false;
            char* str_ptr = gbln_value_as_string(ffi_value, &ok);
            if (!ok || !str_ptr) {
                throw ParseError("Failed to extract string value");
            }
            // String is owned by FFI value, so copy it
            return Value(std::string(str_ptr));
        }
        
        case Object: {
            // Get all keys
            std::size_t count = 0;
            char** keys = gbln_object_keys(ffi_value, &count);
            
            if (!keys && count > 0) {
                throw ParseError("Failed to get object keys");
            }
            
            ManagedKeys managed_keys(keys, count);
            
            // Build C++ map
            std::map<std::string, Value> obj;
            for (std::size_t i = 0; i < count; ++i) {
                const char* key = managed_keys[i];
                const GblnValue* value_ptr = gbln_object_get(ffi_value, key);
                
                if (!value_ptr) {
                    throw ParseError(
                        std::string("Failed to get object value for key: ") + key
                    );
                }
                
                // Recursively convert value
                obj[key] = ffi_to_cpp(value_ptr);
            }
            
            return Value(std::move(obj));
        }
        
        case Array: {
            std::size_t len = gbln_array_len(ffi_value);
            std::vector<Value> arr;
            arr.reserve(len);
            
            for (std::size_t i = 0; i < len; ++i) {
                const GblnValue* elem = gbln_array_get(ffi_value, i);
                
                if (!elem) {
                    throw ParseError(
                        "Failed to get array element at index: " + std::to_string(i)
                    );
                }
                
                // Recursively convert element
                arr.push_back(ffi_to_cpp(elem));
            }
            
            return Value(std::move(arr));
        }
        
        default:
            throw ParseError("Unknown GBLN value type: " + std::to_string(static_cast<int>(type)));
    }
}

/* ========================================================================== */
/* C++ → FFI Conversion Implementation                                        */
/* ========================================================================== */

GblnValue* cpp_to_ffi(const Value& cpp_value) {
    // Use std::visit for type-safe variant handling
    return std::visit([](auto&& arg) -> GblnValue* {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return gbln_value_new_null();
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return gbln_value_new_bool(arg);
        }
        else if constexpr (std::is_same_v<T, std::int64_t>) {
            return create_optimal_int(arg);
        }
        else if constexpr (std::is_same_v<T, double>) {
            return gbln_value_new_f64(arg);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return create_optimal_string(arg);
        }
        else if constexpr (std::is_same_v<T, std::map<std::string, Value>>) {
            // Create object
            GblnValue* obj = gbln_value_new_object();
            if (!obj) {
                throw SerialiseError("Failed to create object");
            }
            
            // Insert all key-value pairs
            for (const auto& [key, value] : arg) {
                GblnValue* ffi_value = cpp_to_ffi(value);
                
                GblnErrorCode err = gbln_object_insert(obj, key.c_str(), ffi_value);
                if (err != Ok) {
                    // Ownership not transferred on error - must free both
                    gbln_value_free(ffi_value);
                    gbln_value_free(obj);
                    throw SerialiseError(
                        "Failed to insert object key: " + key
                    );
                }
                // Success: ownership transferred, don't free ffi_value
            }
            
            return obj;
        }
        else if constexpr (std::is_same_v<T, std::vector<Value>>) {
            // Create array
            GblnValue* arr = gbln_value_new_array();
            if (!arr) {
                throw SerialiseError("Failed to create array");
            }
            
            // Push all elements
            for (const auto& elem : arg) {
                GblnValue* ffi_value = cpp_to_ffi(elem);
                
                GblnErrorCode err = gbln_array_push(arr, ffi_value);
                if (err != Ok) {
                    // Ownership not transferred on error
                    gbln_value_free(ffi_value);
                    gbln_value_free(arr);
                    throw SerialiseError("Failed to push array element");
                }
                // Success: ownership transferred
            }
            
            return arr;
        }
        else {
            throw SerialiseError("Unknown C++ value type");
        }
    }, cpp_value.data);
}

/* ========================================================================== */
/* Auto-Type Selection Implementation                                         */
/* ========================================================================== */

GblnValue* create_optimal_int(std::int64_t value) {
    // Select smallest signed/unsigned type that fits
    
    // Try unsigned types first (more common for IDs, counts, etc.)
    if (value >= 0) {
        if (value <= std::numeric_limits<std::uint8_t>::max()) {
            return gbln_value_new_u8(static_cast<std::uint8_t>(value));
        }
        if (value <= std::numeric_limits<std::uint16_t>::max()) {
            return gbln_value_new_u16(static_cast<std::uint16_t>(value));
        }
        if (value <= std::numeric_limits<std::uint32_t>::max()) {
            return gbln_value_new_u32(static_cast<std::uint32_t>(value));
        }
        return gbln_value_new_u64(static_cast<std::uint64_t>(value));
    }
    
    // Negative: use signed types
    if (value >= std::numeric_limits<std::int8_t>::min() && 
        value <= std::numeric_limits<std::int8_t>::max()) {
        return gbln_value_new_i8(static_cast<std::int8_t>(value));
    }
    if (value >= std::numeric_limits<std::int16_t>::min() && 
        value <= std::numeric_limits<std::int16_t>::max()) {
        return gbln_value_new_i16(static_cast<std::int16_t>(value));
    }
    if (value >= std::numeric_limits<std::int32_t>::min() && 
        value <= std::numeric_limits<std::int32_t>::max()) {
        return gbln_value_new_i32(static_cast<std::int32_t>(value));
    }
    
    return gbln_value_new_i64(value);
}

GblnValue* create_optimal_string(const std::string& str) {
    // Count UTF-8 characters (not bytes)
    std::size_t char_count = 0;
    for (std::size_t i = 0; i < str.size(); ) {
        unsigned char c = str[i];
        
        // UTF-8 character length detection
        if (c < 0x80) {
            i += 1;  // 1-byte character (ASCII)
        } else if ((c & 0xE0) == 0xC0) {
            i += 2;  // 2-byte character
        } else if ((c & 0xF0) == 0xE0) {
            i += 3;  // 3-byte character
        } else if ((c & 0xF8) == 0xF0) {
            i += 4;  // 4-byte character (emoji, etc.)
        } else {
            // Invalid UTF-8
            throw SerialiseError("Invalid UTF-8 sequence in string");
        }
        
        ++char_count;
    }
    
    // Select appropriate string type based on character count
    std::size_t max_len;
    
    if (char_count <= 2) max_len = 2;
    else if (char_count <= 4) max_len = 4;
    else if (char_count <= 8) max_len = 8;
    else if (char_count <= 16) max_len = 16;
    else if (char_count <= 32) max_len = 32;
    else if (char_count <= 64) max_len = 64;
    else if (char_count <= 128) max_len = 128;
    else if (char_count <= 256) max_len = 256;
    else if (char_count <= 512) max_len = 512;
    else if (char_count <= 1024) max_len = 1024;
    else {
        throw SerialiseError(
            "String too long (" + std::to_string(char_count) + 
            " characters, max 1024)"
        );
    }
    
    return gbln_value_new_str(str.c_str(), max_len);
}

} // namespace gbln::detail
