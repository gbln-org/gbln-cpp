# GBLN C++ Bindings

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)](https://en.cppreference.com/w/cpp/17)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![FreeBSD](https://img.shields.io/badge/FreeBSD-x64%20%7C%20ARM64-red)](https://www.freebsd.org/)
[![Linux](https://img.shields.io/badge/Linux-x64%20%7C%20ARM64-blue)](https://www.kernel.org/)
[![macOS](https://img.shields.io/badge/macOS-x64%20%7C%20ARM64-lightgrey)](https://www.apple.com/macos/)
[![Windows](https://img.shields.io/badge/Windows-x64-blue)](https://www.microsoft.com/windows/)

Modern C++17 bindings for **GBLN (Goblin Bounded Lean Notation)** - the first LLM-native serialisation format with type-safe, token-efficient design.

## Features

- 🦺 **Type-Safe**: `std::variant`-based values with compile-time type checking
- 🧹 **RAII**: Automatic memory management with smart pointers
- 🎯 **Modern C++17**: Clean, idiomatic API using STL containers
- ⚡ **Zero-Cost**: Thin wrapper over C FFI with minimal overhead
- 🌍 **Cross-Platform**: Linux, macOS, Windows, FreeBSD (x64 & ARM64)
- 🔒 **Exception-Based**: Clear error handling with detailed messages

## Quick Start

### Installation

```bash
# Clone repository
git clone https://github.com/gbln-org/gbln-cpp.git
cd gbln-cpp

# Build
cmake -B build
cmake --build build

# Install
sudo cmake --install build
```

### Basic Usage

```cpp
#include <gbln/gbln.hpp>
#include <iostream>

int main() {
    // Parse GBLN string
    auto data = gbln::parse("user{id<u32>(12345)name<s64>(Alice)}");
    
    // Access values
    auto& obj = data.as_object();
    std::cout << "Name: " << obj["name"].as_string() << '\n';
    
    // Create values
    gbln::Value new_user = gbln::Value{
        std::map<std::string, gbln::Value>{
            {"id", gbln::Value(67890)},
            {"name", gbln::Value("Bob")}
        }
    };
    
    // Serialise
    std::string gbln_str = gbln::to_string(new_user);
    std::cout << "GBLN: " << gbln_str << '\n';
    
    return 0;
}
```

**Compile:**
```bash
g++ -std=c++17 myapp.cpp -lgbln -o myapp
```

## API Overview

### Parsing

```cpp
// Parse from string
gbln::Value data = gbln::parse("user{id<u32>(123)}");

// Parse from file
gbln::Value data = gbln::parse_file("config.gbln");
```

### Value Access

```cpp
// Type checking
if (data.is_object()) { /* ... */ }
if (data.is_int()) { /* ... */ }

// Value extraction (throws std::bad_variant_access if wrong type)
std::int64_t num = data.as_int();
std::string str = data.as_string();
bool b = data.as_bool();

// Container access
auto& obj = data.as_object();           // std::map<std::string, Value>
auto& arr = data.as_array();            // std::vector<Value>
```

### Value Creation

```cpp
// Primitives
gbln::Value null_val = gbln::Value(nullptr);
gbln::Value bool_val = gbln::Value(true);
gbln::Value int_val = gbln::Value(42);
gbln::Value float_val = gbln::Value(3.14);
gbln::Value str_val = gbln::Value("Hello");

// Objects
gbln::Value obj = gbln::Value{
    std::map<std::string, gbln::Value>{
        {"name", gbln::Value("Alice")},
        {"age", gbln::Value(25)}
    }
};

// Arrays
gbln::Value arr = gbln::Value{
    std::vector<gbln::Value>{
        gbln::Value(1),
        gbln::Value(2),
        gbln::Value(3)
    }
};
```

### Serialisation

```cpp
// Compact format (MINI GBLN)
std::string compact = gbln::to_string(value);

// Pretty format (formatted with newlines)
std::string pretty = gbln::to_string_pretty(value);
```

### I/O Format

```cpp
// Read .io.gbln.xz file
gbln::Value data = gbln::read_io("config.io.gbln.xz");

// Write with custom configuration
gbln::Config config = gbln::Config::io_default();
config.compression_level = 9;  // Maximum compression

gbln::write_io(data, "output.io.gbln.xz", config);
```

### Error Handling

```cpp
try {
    auto data = gbln::parse("invalid{syntax");
} catch (const gbln::ParseError& e) {
    std::cerr << "Parse error: " << e.what() << '\n';
} catch (const gbln::ValidationError& e) {
    std::cerr << "Validation error: " << e.what() << '\n';
} catch (const gbln::IoError& e) {
    std::cerr << "I/O error: " << e.what() << '\n';
} catch (const gbln::Exception& e) {
    std::cerr << "GBLN error: " << e.what() << '\n';
}
```

## Type System

GBLN C++ bindings use `std::variant` for type-safe values:

```cpp
using ValueData = std::variant<
    std::nullptr_t,                     // Null
    bool,                               // Bool
    std::int64_t,                       // Integer (I8-I64, U8-U64)
    double,                             // Float (F32, F64)
    std::string,                        // String
    std::map<std::string, Value>,       // Object
    std::vector<Value>                  // Array
>;
```

### Auto-Type Selection

The library automatically selects optimal GBLN types:

**Integers:**
- 0-255 → `u8`
- 0-65535 → `u16`
- -128-127 → `i8`
- (etc., smallest type that fits)

**Strings:**
- ≤64 chars → `s64`
- ≤256 chars → `s256`
- ≤1024 chars → `s1024`

## Configuration

```cpp
struct Config {
    bool mini_mode = true;           // Use MINI format (compact)
    bool compress = true;            // Enable XZ compression
    int compression_level = 6;       // 0-9 (0=fast, 9=best)
    int indent = 2;                  // Indentation width
    bool strip_comments = true;      // Remove comments
};

// Presets
gbln::Config::io_default();      // MINI + compressed
gbln::Config::source_default();  // Pretty + uncompressed
```

## Examples

See the `examples/` directory:

- **basic_usage.cpp** - Parsing, value access, serialisation
- **error_handling.cpp** - Exception handling patterns

Build examples:
```bash
cmake -B build -DGBLN_BUILD_EXAMPLES=ON
cmake --build build
./build/examples/basic_usage
```

## Requirements

- **C++17 compiler**: GCC 8+, Clang 7+, MSVC 2019+
- **CMake**: 3.20+
- **GBLN C FFI**: Automatically located from `../../core/ffi/libs/`

## Platform Support

This binding inherits full platform support from the C FFI layer:

| Platform | Architecture | Status |
|----------|-------------|--------|
| **FreeBSD** | x86_64, ARM64 | ✅ Supported |
| **Linux** | x86_64, ARM64 | ✅ Supported |
| **macOS** | x86_64, ARM64 | ✅ Supported |
| **Windows** | x86_64 | ✅ Supported |
| **Android** | ARM64, x86_64 | ✅ Supported |

Precompiled `libgbln` binaries are included for all platforms.

## Building from Source

```bash
# Clone with submodules
git clone --recursive https://github.com/gbln-org/gbln.git
cd gbln/bindings/cpp

# Build C FFI first (if not already built)
cd ../../core/ffi
cargo build --release
cd ../../bindings/cpp

# Build C++ bindings
cmake -B build
cmake --build build

# Run tests
ctest --test-dir build

# Install
sudo cmake --install build
```

## CMake Integration

### Using find_package

```cmake
find_package(gbln REQUIRED)
target_link_libraries(myapp PRIVATE gbln::gbln)
```

### Using FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(gbln
    GIT_REPOSITORY https://github.com/gbln-org/gbln-cpp
    GIT_TAG v0.1.0
)
FetchContent_MakeAvailable(gbln)
target_link_libraries(myapp PRIVATE gbln::gbln)
```

## Performance

- **Parse Speed**: ~65ms for 1000 records (via C FFI)
- **Memory Overhead**: <10% vs direct C usage (RAII efficiency)
- **Size**: 70% smaller than JSON, 40% smaller than Protobuf

## Architecture

```
Rust Core → C FFI (libgbln) → C++ Wrapper → Your Application
```

All GBLN language bindings use the same C FFI layer for consistency and performance.

## Documentation

- **API Reference**: See headers in `include/gbln/`
- **Examples**: See `examples/` directory
- **Specification**: [../../docs/01-specification.md](../../docs/01-specification.md)
- **Website**: https://gbln.dev (coming soon)

## Contributing

Contributions welcome! Please follow:

- **BBC English** in all comments (`initialise`, `serialise`, not `initialize`, `serialize`)
- **KISS principle**: Files <400 lines, single responsibility
- **C++17 standard**: Modern, idiomatic C++
- **Tests**: Add tests for new features

## License

Apache License 2.0 - see [LICENSE](LICENSE)

## Links

- **Main Repository**: https://github.com/gbln-org/gbln
- **C FFI Layer**: https://github.com/gbln-org/gbln-rust
- **Issue Tracker**: https://github.com/gbln-org/gbln-cpp/issues
- **Specification**: [GBLN Specification](../../docs/01-specification.md)

---

**GBLN** - *Type-safe data that speaks clearly* 🦇
