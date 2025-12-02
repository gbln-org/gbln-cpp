// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file basic_usage.cpp
 * @brief Basic GBLN usage example
 * 
 * Demonstrates parsing, value access, and serialisation.
 */

#include <gbln/gbln.hpp>
#include <iostream>

int main() {
    try {
        std::cout << "=== GBLN C++ Bindings - Basic Usage Example ===\n\n";
        
        // Parse GBLN string
        std::cout << "1. Parsing GBLN string...\n";
        auto data = gbln::parse("user{id<u32>(12345)name<s64>(Alice)age<i8>(25)active<b>(t)}");
        
        std::cout << "   ✓ Parsed successfully\n\n";
        
        // Access values
        std::cout << "2. Accessing values...\n";
        
        // Note: GBLN parses "user{...}" as a record with identifier "user"
        // So the top level is an object with key "user"
        if (data.is_object()) {
            auto& root = data.as_object();
            auto& user = root["user"].as_object();
            
            std::cout << "   User ID: " << user["id"].as_int() << "\n";
            std::cout << "   Name: " << user["name"].as_string() << "\n";
            std::cout << "   Age: " << user["age"].as_int() << "\n";
            std::cout << "   Active: " << (user["active"].as_bool() ? "true" : "false") << "\n";
        }
        
        std::cout << "\n";
        
        // Create new value
        std::cout << "3. Creating new value from C++...\n";
        
        // Create a GBLN record structure (identifier -> object)
        gbln::Value new_data = gbln::Value{
            std::map<std::string, gbln::Value>{
                {"user", gbln::Value{
                    std::map<std::string, gbln::Value>{
                        {"id", gbln::Value(67890)},
                        {"name", gbln::Value("Bob")},
                        {"age", gbln::Value(30)},
                        {"active", gbln::Value(false)}
                    }
                }}
            }
        };
        
        std::cout << "   ✓ Value created\n\n";
        
        // Serialise back to GBLN
        std::cout << "4. Serialising to GBLN...\n";
        
        std::string compact = gbln::to_string(new_data);
        std::cout << "   Compact: " << compact << "\n\n";
        
        std::string pretty = gbln::to_string_pretty(new_data);
        std::cout << "   Pretty:\n" << pretty << "\n\n";
        
        // Round-trip test
        std::cout << "5. Round-trip test...\n";
        auto reparsed = gbln::parse(compact);
        std::string round_trip = gbln::to_string(reparsed);
        
        if (compact == round_trip) {
            std::cout << "   ✓ Round-trip successful!\n";
        } else {
            std::cout << "   ✗ Round-trip failed\n";
            std::cout << "     Original: " << compact << "\n";
            std::cout << "     After:    " << round_trip << "\n";
        }
        
        std::cout << "\n=== Example completed successfully ===\n";
        return 0;
        
    } catch (const gbln::ParseError& e) {
        std::cerr << "Parse error: " << e.what() << "\n";
        return 1;
    } catch (const gbln::Exception& e) {
        std::cerr << "GBLN error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
