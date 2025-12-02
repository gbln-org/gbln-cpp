// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file config.hpp
 * @brief Configuration for GBLN I/O operations
 * 
 * Provides clean configuration struct with sensible defaults.
 */

#pragma once

#include "exceptions.hpp"

namespace gbln {

/// Configuration for GBLN I/O operations.
/// Clean struct with sensible defaults matching C FFI layer.
struct Config {
    /// Use MINI GBLN format (no whitespace, compact)
    bool mini_mode = true;
    
    /// Enable XZ compression for I/O format
    bool compress = true;
    
    /// XZ compression level (0-9, where 0=fastest, 9=best compression)
    int compression_level = 6;
    
    /// Indentation width for pretty format (ignored if mini_mode=true)
    int indent = 2;
    
    /// Remove comments in I/O files
    bool strip_comments = true;
    
    /// Validate configuration.
    /// @throws ValidationError if configuration is invalid
    void validate() const {
        if (compression_level < 0 || compression_level > 9) {
            throw ValidationError(
                "Compression level must be 0-9, got: " + 
                std::to_string(compression_level)
            );
        }
        
        if (indent < 0 || indent > 16) {
            throw ValidationError(
                "Indent must be 0-16, got: " + 
                std::to_string(indent)
            );
        }
    }
    
    /// Create default configuration for I/O format
    /// (MINI + compressed)
    static Config io_default() {
        Config cfg;
        cfg.mini_mode = true;
        cfg.compress = true;
        cfg.compression_level = 6;
        cfg.strip_comments = true;
        return cfg;
    }
    
    /// Create default configuration for source format
    /// (Pretty + uncompressed)
    static Config source_default() {
        Config cfg;
        cfg.mini_mode = false;
        cfg.compress = false;
        cfg.indent = 2;
        cfg.strip_comments = false;
        return cfg;
    }
};

} // namespace gbln
