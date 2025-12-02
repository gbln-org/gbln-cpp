# Copyright (c) 2025 Vivian Burkhard Voss
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20)
project(gbln-cpp VERSION 0.9.0 LANGUAGES CXX)

# C++17 minimum, C++20 recommended
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Platform detection
if(APPLE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        set(GBLN_PLATFORM "macos-arm64")
        set(GBLN_LIB_NAME "libgbln.dylib")
    else()
        set(GBLN_PLATFORM "macos-x64")
        set(GBLN_LIB_NAME "libgbln.dylib")
    endif()
elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
            set(GBLN_PLATFORM "freebsd-arm64")
        else()
            set(GBLN_PLATFORM "freebsd-x64")
        endif()
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
            set(GBLN_PLATFORM "linux-arm64")
        else()
            set(GBLN_PLATFORM "linux-x64")
        endif()
    endif()
    set(GBLN_LIB_NAME "libgbln.so")
elseif(WIN32)
    set(GBLN_PLATFORM "windows-x64")
    set(GBLN_LIB_NAME "gbln.dll")
endif()

message(STATUS "Detected platform: ${GBLN_PLATFORM}")

# Locate GBLN C FFI library
set(GBLN_FFI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../core/ffi")
set(GBLN_LIBRARY "${GBLN_FFI_DIR}/libs/${GBLN_PLATFORM}/${GBLN_LIB_NAME}")
set(GBLN_INCLUDE_DIR "${GBLN_FFI_DIR}/include")

if(NOT EXISTS "${GBLN_LIBRARY}")
    message(FATAL_ERROR "GBLN library not found: ${GBLN_LIBRARY}")
endif()

if(NOT EXISTS "${GBLN_INCLUDE_DIR}/gbln.h")
    message(FATAL_ERROR "GBLN header not found: ${GBLN_INCLUDE_DIR}/gbln.h")
endif()

message(STATUS "Using GBLN library: ${GBLN_LIBRARY}")

# Create library
add_library(gbln
    src/parser.cpp
    src/serialiser.cpp
    src/io.cpp
    src/conversion.cpp
)

# Include directories
target_include_directories(gbln
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${GBLN_INCLUDE_DIR}
)

# Link against C FFI library
target_link_libraries(gbln
    PRIVATE ${GBLN_LIBRARY}
)

# Set RPATH for finding libgbln at runtime
if(APPLE)
    set_target_properties(gbln PROPERTIES
        INSTALL_RPATH "@loader_path/../../../core/ffi/libs/${GBLN_PLATFORM}"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
elseif(UNIX)
    set_target_properties(gbln PROPERTIES
        INSTALL_RPATH "$ORIGIN/../../../core/ffi/libs/${GBLN_PLATFORM}"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

# Install targets
install(TARGETS gbln
    EXPORT gbln-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/gbln
    DESTINATION include
)

install(EXPORT gbln-targets
    FILE gbln-targets.cmake
    NAMESPACE gbln::
    DESTINATION lib/cmake/gbln
)

# Create config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gbln-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/gbln-config.cmake"
    INSTALL_DESTINATION lib/cmake/gbln
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/gbln-config.cmake"
    DESTINATION lib/cmake/gbln
)

# Examples (optional)
option(GBLN_BUILD_EXAMPLES "Build examples" ON)
if(GBLN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Testing (optional)
# TODO: Implement tests
# option(GBLN_BUILD_TESTS "Build tests" ON)
# if(GBLN_BUILD_TESTS)
#     enable_testing()
#     add_subdirectory(tests)
# endif()
