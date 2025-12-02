// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file exceptions.hpp
 * @brief Exception types for GBLN C++ bindings
 * 
 * Provides clean C++ exception hierarchy for error handling.
 */

#pragma once

#include <stdexcept>
#include <string>

namespace gbln {

/// Base exception for all GBLN errors.
/// Inherits from std::runtime_error for standard exception handling.
class Exception : public std::runtime_error {
public:
    explicit Exception(const std::string& message)
        : std::runtime_error(message) {}
};

/// Thrown when parsing fails.
/// Indicates syntax errors, unexpected tokens, or malformed GBLN data.
class ParseError : public Exception {
public:
    explicit ParseError(const std::string& message)
        : Exception("Parse error: " + message) {}
};

/// Thrown when validation fails.
/// Indicates type mismatches, out-of-range values, or invalid constraints.
class ValidationError : public Exception {
public:
    explicit ValidationError(const std::string& message)
        : Exception("Validation error: " + message) {}
};

/// Thrown when I/O operations fail.
/// Indicates file not found, permission errors, or read/write failures.
class IoError : public Exception {
public:
    explicit IoError(const std::string& message)
        : Exception("I/O error: " + message) {}
};

/// Thrown when serialisation fails.
/// Indicates failure to convert C++ values to GBLN format.
class SerialiseError : public Exception {
public:
    explicit SerialiseError(const std::string& message)
        : Exception("Serialise error: " + message) {}
};

} // namespace gbln
