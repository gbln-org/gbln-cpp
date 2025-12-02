// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

#include "gbln/parser.hpp"
#include "gbln/detail/ffi.hpp"
#include "gbln/detail/managed_value.hpp"
#include "gbln/detail/conversion.hpp"
#include <fstream>
#include <sstream>

namespace gbln {

Value parse(const std::string& gbln_string) {
    GblnValue* out_value = nullptr;
    GblnErrorCode result = gbln_parse(gbln_string.c_str(), &out_value);
    
    if (result != Ok) {
        // Get error details
        char* error_msg = gbln_last_error_message();
        std::string msg = error_msg ? std::string(error_msg) : "Parse failed";
        
        if (error_msg) {
            gbln_string_free(error_msg);
        }
        
        throw ParseError(msg);
    }
    
    if (!out_value) {
        throw ParseError("Parse returned null pointer");
    }
    
    // Wrap in RAII for automatic cleanup
    detail::ManagedValue managed(out_value);
    
    // Convert to C++ Value
    return detail::ffi_to_cpp(managed.get());
}

Value parse_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw IoError("Cannot open file: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    if (file.bad()) {
        throw IoError("Error reading file: " + path);
    }
    
    return parse(buffer.str());
}

Value parse_file(const std::filesystem::path& path) {
    return parse_file(path.string());
}

} // namespace gbln
