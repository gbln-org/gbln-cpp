// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file conversion.hpp
 * @brief Bidirectional conversion between C FFI and C++ types
 * 
 * Provides conversion functions for GBLN FFI ↔ C++ Value types.
 */

#pragma once

#include "ffi.hpp"
#include "managed_value.hpp"
#include "../value.hpp"
#include "../exceptions.hpp"

namespace gbln::detail {

/* ========================================================================== */
/* FFI → C++ Conversion                                                       */
/* ========================================================================== */

/// Convert C FFI GblnValue to C++ Value.
/// Recursively handles objects and arrays.
///
/// @param ffi_value FFI value to convert (borrowed reference)
/// @return C++ Value
/// @throws ParseError if conversion fails
Value ffi_to_cpp(const GblnValue* ffi_value);

/* ========================================================================== */
/* C++ → FFI Conversion                                                       */
/* ========================================================================== */

/// Convert C++ Value to C FFI GblnValue.
/// Recursively handles objects and arrays.
/// Caller owns the returned value and must free it.
///
/// @param cpp_value C++ value to convert
/// @return FFI value (caller owns, must call gbln_value_free)
/// @throws SerialiseError if conversion fails
GblnValue* cpp_to_ffi(const Value& cpp_value);

/* ========================================================================== */
/* Auto-Type Selection Helpers                                                */
/* ========================================================================== */

/// Select smallest integer type that fits the value.
/// Used for automatic type selection from C++ int64_t.
///
/// @param value Integer value
/// @return FFI value with optimal type
GblnValue* create_optimal_int(std::int64_t value);

/// Select optimal string type based on length.
/// Used for automatic max_len selection.
///
/// @param str String value
/// @return FFI value with optimal string type
GblnValue* create_optimal_string(const std::string& str);

} // namespace gbln::detail
