// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file parser.hpp
 * @brief GBLN parser API
 * 
 * Provides clean C++ interface for parsing GBLN data.
 */

#pragma once

#include "value.hpp"
#include "exceptions.hpp"
#include <string>
#include <filesystem>

namespace gbln {

/// Parse GBLN string to Value.
///
/// @param gbln_string GBLN-formatted string
/// @return Parsed GBLN value
/// @throws ParseError if parsing fails
///
/// @example
/// @code
/// auto data = gbln::parse("user{id<u32>(123)name<s32>(Alice)}");
/// auto& obj = data.as_object();
/// std::cout << obj["name"].as_string() << '\n';
/// @endcode
Value parse(const std::string& gbln_string);

/// Parse GBLN file to Value.
///
/// @param path Path to .gbln file
/// @return Parsed GBLN value
/// @throws ParseError if parsing fails
/// @throws IoError if file cannot be read
Value parse_file(const std::string& path);

/// Parse GBLN file to Value (filesystem::path overload).
///
/// @param path Path to .gbln file
/// @return Parsed GBLN value
/// @throws ParseError if parsing fails
/// @throws IoError if file cannot be read
Value parse_file(const std::filesystem::path& path);

} // namespace gbln
