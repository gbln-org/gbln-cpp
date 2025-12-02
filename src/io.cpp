// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

#include "gbln/io.hpp"
#include "gbln/detail/ffi.hpp"
#include "gbln/detail/managed_value.hpp"
#include "gbln/detail/conversion.hpp"

namespace gbln {

Value read_io(const std::filesystem::path& path) {
    std::string path_str = path.string();
    GblnValue* out_value = nullptr;
    GblnErrorCode result = gbln_read_io(path_str.c_str(), &out_value);
    
    if (result != Ok) {
        char* error_msg = gbln_last_error_message();
        std::string msg = error_msg ? std::string(error_msg) : "I/O read failed";
        
        if (error_msg) {
            gbln_string_free(error_msg);
        }
        
        throw IoError("Failed to read I/O file '" + path_str + "': " + msg);
    }
    
    if (!out_value) {
        throw IoError("I/O read returned null pointer for: " + path_str);
    }
    
    // Wrap for automatic cleanup
    detail::ManagedValue managed(out_value);
    
    // Convert to C++ value
    return detail::ffi_to_cpp(managed.get());
}

void write_io(const Value& value, const std::filesystem::path& path, const Config& config) {
    // Validate config
    config.validate();
    
    // Convert C++ value to FFI
    GblnValue* ffi_value = detail::cpp_to_ffi(value);
    detail::ManagedValue managed_value(ffi_value);
    
    // Create FFI config
    GblnConfig* ffi_config = gbln_config_new(
        config.mini_mode,
        config.compress,
        static_cast<std::uint8_t>(config.compression_level),
        static_cast<std::size_t>(config.indent),
        config.strip_comments
    );
    
    if (!ffi_config) {
        throw IoError("Failed to create configuration");
    }
    
    detail::ManagedConfig managed_config(ffi_config);
    
    std::string path_str = path.string();
    
    // Write to file
    GblnErrorCode result = gbln_write_io(
        managed_value.get(),
        path_str.c_str(),
        managed_config.get()
    );
    
    if (result != Ok) {
        char* error_msg = gbln_last_error_message();
        std::string msg = error_msg ? std::string(error_msg) : "I/O write failed";
        
        if (error_msg) {
            gbln_string_free(error_msg);
        }
        
        throw IoError("Failed to write I/O file '" + path_str + "': " + msg);
    }
}

} // namespace gbln
