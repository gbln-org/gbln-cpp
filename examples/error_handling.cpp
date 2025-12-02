// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file error_handling.cpp
 * @brief GBLN error handling example
 * 
 * Demonstrates exception-based error handling in GBLN C++ bindings.
 */

#include <gbln/gbln.hpp>
#include <iostream>

void test_parse_error() {
    std::cout << "Testing parse error...\n";
    
    try {
        // Invalid syntax - missing closing brace
        auto data = gbln::parse("user{id<u32>(123)");
        std::cout << "  ✗ Should have thrown ParseError!\n";
    } catch (const gbln::ParseError& e) {
        std::cout << "  ✓ Caught ParseError: " << e.what() << "\n";
    }
}

void test_type_error() {
    std::cout << "\nTesting type error...\n";
    
    try {
        auto data = gbln::parse("value<i32>(42)");
        
        // Try to access as wrong type
        std::string str = data.as_string();  // Should throw std::bad_variant_access
        
        std::cout << "  ✗ Should have thrown exception!\n";
    } catch (const std::bad_variant_access& e) {
        std::cout << "  ✓ Caught bad_variant_access: " << e.what() << "\n";
    } catch (const gbln::Exception& e) {
        std::cout << "  ✓ Caught GBLN exception: " << e.what() << "\n";
    }
}

void test_validation_error() {
    std::cout << "\nTesting validation error...\n";
    
    try {
        // Invalid configuration
        gbln::Config bad_config;
        bad_config.compression_level = 99;  // Invalid: must be 0-9
        
        bad_config.validate();
        std::cout << "  ✗ Should have thrown ValidationError!\n";
    } catch (const gbln::ValidationError& e) {
        std::cout << "  ✓ Caught ValidationError: " << e.what() << "\n";
    }
}

void test_io_error() {
    std::cout << "\nTesting I/O error...\n";
    
    try {
        // Try to read non-existent file
        auto data = gbln::read_io("/nonexistent/path/to/file.io.gbln.xz");
        std::cout << "  ✗ Should have thrown IoError!\n";
    } catch (const gbln::IoError& e) {
        std::cout << "  ✓ Caught IoError: " << e.what() << "\n";
    }
}

void test_nested_access() {
    std::cout << "\nTesting nested access...\n";
    
    try {
        auto data = gbln::parse("user{profile{name<s32>(Alice)age<i8>(25)}}");
        
        // Safely access nested structure
        if (data.is_object()) {
            auto& obj = data.as_object();
            
            if (obj.count("profile") > 0 && obj["profile"].is_object()) {
                auto& profile = obj["profile"].as_object();
                
                if (profile.count("name") > 0) {
                    std::cout << "  Name: " << profile["name"].as_string() << "\n";
                }
                
                if (profile.count("age") > 0) {
                    std::cout << "  Age: " << profile["age"].as_int() << "\n";
                }
            }
        }
        
        std::cout << "  ✓ Nested access successful\n";
        
    } catch (const std::exception& e) {
        std::cout << "  ✗ Error: " << e.what() << "\n";
    }
}

int main() {
    std::cout << "=== GBLN C++ Bindings - Error Handling Example ===\n\n";
    
    test_parse_error();
    test_type_error();
    test_validation_error();
    test_io_error();
    test_nested_access();
    
    std::cout << "\n=== All error handling tests completed ===\n";
    return 0;
}
