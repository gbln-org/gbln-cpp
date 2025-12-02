// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file io.hpp
 * @brief GBLN I/O format operations
 * 
 * Provides read/write operations for .io.gbln.xz format.
 */

#pragma once

#include "value.hpp"
#include "config.hpp"
#include "exceptions.hpp"
#include <string>
#include <filesystem>

namespace gbln {

/// Read GBLN I/O format file (.io.gbln.xz).
///
/// @param path Path to .io.gbln.xz file
/// @return Parsed GBLN value
/// @throws IoError if file cannot be read
/// @throws ParseError if content is invalid
///
/// @example
/// @code
/// auto data = gbln::read_io("config.io.gbln.xz");
/// @endcode
Value read_io(const std::filesystem::path& path);

/// Write GBLN I/O format file (.io.gbln.xz).
///
/// @param value Value to write
/// @param path Output file path
/// @param config Configuration options (default: MINI + compressed)
/// @throws IoError if file cannot be written
/// @throws SerialiseError if serialisation fails
///
/// @example
/// @code
/// Value data = /* ... */;
/// gbln::write_io(data, "output.io.gbln.xz");
/// @endcode
void write_io(const Value& value, const std::filesystem::path& path,
              const Config& config = Config::io_default());

} // namespace gbln
