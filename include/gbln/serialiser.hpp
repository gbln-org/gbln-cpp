// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file serialiser.hpp
 * @brief GBLN serialiser API
 * 
 * Provides clean C++ interface for serialising values to GBLN format.
 */

#pragma once

#include "value.hpp"
#include "exceptions.hpp"
#include <string>

namespace gbln {

/// Serialise Value to GBLN string.
///
/// @param value Value to serialise
/// @param mini Use MINI format (compact, no whitespace)
/// @return GBLN-formatted string
/// @throws SerialiseError if serialisation fails
///
/// @example
/// @code
/// Value v = Value{std::map<std::string, Value>{
///     {"name", Value("Alice")},
///     {"age", Value(25)}
/// }};
/// std::string gbln_str = gbln::to_string(v);
/// // Result: "age<u8>(25)name<s8>(Alice)"
/// @endcode
std::string to_string(const Value& value, bool mini = true);

/// Serialise Value to pretty GBLN string.
/// Convenience function for `to_string(value, false)` with formatting.
///
/// @param value Value to serialise
/// @param indent Indentation width (default: 2)
/// @return Pretty-formatted GBLN string with newlines and indentation
/// @throws SerialiseError if serialisation fails
///
/// @example
/// @code
/// Value v = Value{std::map<std::string, Value>{
///     {"name", Value("Alice")},
///     {"age", Value(25)}
/// }};
/// std::string pretty = gbln::to_string_pretty(v);
/// // Result (formatted):
/// // age<u8>(25)
/// // name<s8>(Alice)
/// @endcode
std::string to_string_pretty(const Value& value, int indent = 2);

} // namespace gbln
