// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file value.hpp
 * @brief Type-safe GBLN value representation
 * 
 * Provides std::variant-based value type for clean, modern C++17 API.
 */

#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace gbln {

// Forward declaration for recursive type
struct Value;

/// Type-safe GBLN value using std::variant.
/// Provides clean, modern C++17 implementation with zero-cost abstractions.
///
/// Supported types:
/// - std::nullptr_t: GBLN Null
/// - bool: GBLN Bool
/// - std::int64_t: GBLN integer types (I8-I64, U8-U64)
/// - double: GBLN float types (F32, F64)
/// - std::string: GBLN Str
/// - std::map<std::string, Value>: GBLN Object
/// - std::vector<Value>: GBLN Array
using ValueData = std::variant<
    std::nullptr_t,                     // Null
    bool,                               // Bool
    std::int64_t,                       // Integer (all int types)
    double,                             // Float (F32, F64)
    std::string,                        // String
    std::map<std::string, Value>,       // Object
    std::vector<Value>                  // Array
>;

/// GBLN value wrapper.
/// Provides convenient access to variant with type checking methods.
struct Value {
    /// Underlying variant data
    ValueData data;
    
    /* ====================================================================== */
    /* Constructors                                                           */
    /* ====================================================================== */
    
    /// Default constructor (creates Null value)
    Value() : data(nullptr) {}
    
    /// Null constructor
    Value(std::nullptr_t) : data(nullptr) {}
    
    /// Boolean constructor
    Value(bool b) : data(b) {}
    
    /// 64-bit signed integer constructor
    Value(std::int64_t i) : data(i) {}
    
    /// 32-bit signed integer constructor (auto-converted to int64_t)
    Value(int i) : data(static_cast<std::int64_t>(i)) {}
    
    /// Double precision float constructor
    Value(double d) : data(d) {}
    
    /// String constructor (copy)
    Value(const std::string& s) : data(s) {}
    
    /// String constructor (move)
    Value(std::string&& s) : data(std::move(s)) {}
    
    /// C-string constructor
    Value(const char* s) : data(std::string(s)) {}
    
    /// Object constructor (copy)
    Value(const std::map<std::string, Value>& m) : data(m) {}
    
    /// Object constructor (move)
    Value(std::map<std::string, Value>&& m) : data(std::move(m)) {}
    
    /// Array constructor (copy)
    Value(const std::vector<Value>& v) : data(v) {}
    
    /// Array constructor (move)
    Value(std::vector<Value>&& v) : data(std::move(v)) {}
    
    /* ====================================================================== */
    /* Type Checking                                                          */
    /* ====================================================================== */
    
    /// Check if value is Null
    bool is_null() const { 
        return std::holds_alternative<std::nullptr_t>(data); 
    }
    
    /// Check if value is Bool
    bool is_bool() const { 
        return std::holds_alternative<bool>(data); 
    }
    
    /// Check if value is Integer
    bool is_int() const { 
        return std::holds_alternative<std::int64_t>(data); 
    }
    
    /// Check if value is Float
    bool is_float() const { 
        return std::holds_alternative<double>(data); 
    }
    
    /// Check if value is String
    bool is_string() const { 
        return std::holds_alternative<std::string>(data); 
    }
    
    /// Check if value is Object
    bool is_object() const { 
        return std::holds_alternative<std::map<std::string, Value>>(data); 
    }
    
    /// Check if value is Array
    bool is_array() const { 
        return std::holds_alternative<std::vector<Value>>(data); 
    }
    
    /* ====================================================================== */
    /* Value Access (const)                                                   */
    /* ====================================================================== */
    
    /// Get boolean value
    /// @throws std::bad_variant_access if not Bool
    bool as_bool() const { 
        return std::get<bool>(data); 
    }
    
    /// Get integer value
    /// @throws std::bad_variant_access if not Integer
    std::int64_t as_int() const { 
        return std::get<std::int64_t>(data); 
    }
    
    /// Get float value
    /// @throws std::bad_variant_access if not Float
    double as_float() const { 
        return std::get<double>(data); 
    }
    
    /// Get string value (const reference)
    /// @throws std::bad_variant_access if not String
    const std::string& as_string() const { 
        return std::get<std::string>(data); 
    }
    
    /// Get object value (const reference)
    /// @throws std::bad_variant_access if not Object
    const std::map<std::string, Value>& as_object() const { 
        return std::get<std::map<std::string, Value>>(data); 
    }
    
    /// Get array value (const reference)
    /// @throws std::bad_variant_access if not Array
    const std::vector<Value>& as_array() const { 
        return std::get<std::vector<Value>>(data); 
    }
    
    /* ====================================================================== */
    /* Mutable Access                                                         */
    /* ====================================================================== */
    
    /// Get mutable object reference
    /// @throws std::bad_variant_access if not Object
    std::map<std::string, Value>& as_object() { 
        return std::get<std::map<std::string, Value>>(data); 
    }
    
    /// Get mutable array reference
    /// @throws std::bad_variant_access if not Array
    std::vector<Value>& as_array() { 
        return std::get<std::vector<Value>>(data); 
    }
};

} // namespace gbln
