// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file gbln.hpp
 * @brief Main header for GBLN C++ bindings
 * 
 * Include this single header to access all GBLN functionality.
 * 
 * @example
 * @code
 * #include <gbln/gbln.hpp>
 * #include <iostream>
 * 
 * int main() {
 *     // Parse GBLN
 *     auto data = gbln::parse("user{id<u32>(123)name<s32>(Alice)}");
 *     
 *     // Access values
 *     auto& obj = data.as_object();
 *     std::cout << "Name: " << obj["name"].as_string() << '\n';
 *     
 *     // Serialise back
 *     std::string gbln_str = gbln::to_string(data);
 *     std::cout << "GBLN: " << gbln_str << '\n';
 *     
 *     return 0;
 * }
 * @endcode
 */

#pragma once

// Version information
#define GBLN_VERSION_MAJOR 0
#define GBLN_VERSION_MINOR 9
#define GBLN_VERSION_PATCH 0
#define GBLN_VERSION "0.9.0"

// Core types
#include "value.hpp"
#include "config.hpp"
#include "exceptions.hpp"

// API functions
#include "parser.hpp"
#include "serialiser.hpp"
#include "io.hpp"

/**
 * @namespace gbln
 * @brief Main namespace for GBLN C++ bindings
 * 
 * All GBLN functionality is contained within this namespace.
 */

/**
 * @mainpage GBLN C++ Bindings
 * 
 * @section intro_sec Introduction
 * 
 * GBLN (Goblin Bounded Lean Notation) C++ bindings provide a modern C++17
 * interface for working with GBLN data format.
 * 
 * @section features_sec Key Features
 * 
 * - Type-safe values using std::variant
 * - RAII-based automatic memory management
 * - Exception-based error handling
 * - STL integration (std::map, std::vector, std::string)
 * - Zero-cost abstractions over C FFI
 * 
 * @section usage_sec Basic Usage
 * 
 * @code
 * #include <gbln/gbln.hpp>
 * 
 * // Parse GBLN string
 * auto data = gbln::parse("user{id<u32>(123)name<s32>(Alice)}");
 * 
 * // Access values
 * auto& obj = data.as_object();
 * std::cout << obj["name"].as_string() << '\n';
 * 
 * // Create values
 * gbln::Value new_data = gbln::Value{
 *     std::map<std::string, gbln::Value>{
 *         {"id", gbln::Value(456)},
 *         {"name", gbln::Value("Bob")}
 *     }
 * };
 * 
 * // Serialise
 * std::string gbln_str = gbln::to_string(new_data);
 * @endcode
 * 
 * @section error_handling_sec Error Handling
 * 
 * All errors are reported via exceptions:
 * 
 * @code
 * try {
 *     auto data = gbln::parse("invalid{syntax");
 * } catch (const gbln::ParseError& e) {
 *     std::cerr << "Parse error: " << e.what() << '\n';
 * } catch (const gbln::Exception& e) {
 *     std::cerr << "GBLN error: " << e.what() << '\n';
 * }
 * @endcode
 */
