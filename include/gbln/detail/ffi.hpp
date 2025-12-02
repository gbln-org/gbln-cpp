// Copyright (c) 2025 Vivian Burkhard Voss
// SPDX-License-Identifier: Apache-2.0

/**
 * @file ffi.hpp
 * @brief C FFI declarations for GBLN
 * 
 * Provides C++ declarations for C FFI functions from libgbln.
 * All language bindings use this C FFI layer.
 */

#pragma once

#include <cstdint>
#include <cstddef>

extern "C" {

/* ========================================================================== */
/* Opaque Types                                                               */
/* ========================================================================== */

/// Opaque pointer to C FFI GblnValue
typedef struct GblnValue GblnValue;

/// Opaque pointer to C FFI GblnConfig
typedef struct GblnConfig GblnConfig;

/* ========================================================================== */
/* Error Handling                                                             */
/* ========================================================================== */

/// C FFI error codes
typedef enum {
    Ok = 0,
    ErrorUnexpectedChar = 1,
    ErrorUnterminatedString = 2,
    ErrorUnexpectedToken = 3,
    ErrorUnexpectedEof = 4,
    ErrorInvalidSyntax = 5,
    ErrorIntOutOfRange = 6,
    ErrorStringTooLong = 7,
    ErrorTypeMismatch = 8,
    ErrorInvalidTypeHint = 9,
    ErrorDuplicateKey = 10,
    ErrorNullPointer = 11,
    ErrorIo = 12,
} GblnErrorCode;

/// Value type enum
typedef enum {
    I8 = 0, I16 = 1, I32 = 2, I64 = 3,
    U8 = 4, U16 = 5, U32 = 6, U64 = 7,
    F32 = 8, F64 = 9,
    Str = 10, Bool = 11, Null = 12,
    Object = 13, Array = 14,
} GblnValueType;

/* ========================================================================== */
/* Memory Management                                                          */
/* ========================================================================== */

/// Free GBLN value
void gbln_value_free(GblnValue* value);

/// Free GBLN string
void gbln_string_free(char* s);

/// Free keys array from gbln_object_keys
void gbln_keys_free(char** keys, std::size_t count);

/* ========================================================================== */
/* Parsing                                                                    */
/* ========================================================================== */

/// Parse GBLN string
GblnErrorCode gbln_parse(const char* input, GblnValue** out_value);

/* ========================================================================== */
/* Serialisation                                                              */
/* ========================================================================== */

/// Serialise to compact string
char* gbln_to_string(const GblnValue* value);

/// Serialise to pretty string
char* gbln_to_string_pretty(const GblnValue* value);

/* ========================================================================== */
/* Error Information                                                          */
/* ========================================================================== */

/// Get last error message
char* gbln_last_error_message();

/// Get last error suggestion
char* gbln_last_error_suggestion();

/* ========================================================================== */
/* Type Introspection                                                         */
/* ========================================================================== */

/// Get value type
GblnValueType gbln_value_type(const GblnValue* value);

/// Check if value is null
bool gbln_value_is_null(const GblnValue* value);

/* ========================================================================== */
/* Value Extraction                                                           */
/* ========================================================================== */

/// Extract integer values
std::int8_t gbln_value_as_i8(const GblnValue* value, bool* ok);
std::int16_t gbln_value_as_i16(const GblnValue* value, bool* ok);
std::int32_t gbln_value_as_i32(const GblnValue* value, bool* ok);
std::int64_t gbln_value_as_i64(const GblnValue* value, bool* ok);

/// Extract unsigned integer values
std::uint8_t gbln_value_as_u8(const GblnValue* value, bool* ok);
std::uint16_t gbln_value_as_u16(const GblnValue* value, bool* ok);
std::uint32_t gbln_value_as_u32(const GblnValue* value, bool* ok);
std::uint64_t gbln_value_as_u64(const GblnValue* value, bool* ok);

/// Extract float values
float gbln_value_as_f32(const GblnValue* value, bool* ok);
double gbln_value_as_f64(const GblnValue* value, bool* ok);

/// Extract string value (caller must NOT free - owned by value)
char* gbln_value_as_string(const GblnValue* value, bool* ok);

/// Extract boolean value
bool gbln_value_as_bool(const GblnValue* value, bool* ok);

/* ========================================================================== */
/* Object Operations                                                          */
/* ========================================================================== */

/// Get object length
std::size_t gbln_object_len(const GblnValue* value);

/// Get value from object by key (returns borrowed reference)
const GblnValue* gbln_object_get(const GblnValue* value, const char* key);

/// Get all object keys (caller must free with gbln_keys_free)
char** gbln_object_keys(const GblnValue* value, std::size_t* out_count);

/* ========================================================================== */
/* Array Operations                                                           */
/* ========================================================================== */

/// Get array length
std::size_t gbln_array_len(const GblnValue* value);

/// Get array element by index (returns borrowed reference)
const GblnValue* gbln_array_get(const GblnValue* value, std::size_t index);

/* ========================================================================== */
/* Value Construction                                                         */
/* ========================================================================== */

/// Create integer values
GblnValue* gbln_value_new_i8(std::int8_t value);
GblnValue* gbln_value_new_i16(std::int16_t value);
GblnValue* gbln_value_new_i32(std::int32_t value);
GblnValue* gbln_value_new_i64(std::int64_t value);

/// Create unsigned integer values
GblnValue* gbln_value_new_u8(std::uint8_t value);
GblnValue* gbln_value_new_u16(std::uint16_t value);
GblnValue* gbln_value_new_u32(std::uint32_t value);
GblnValue* gbln_value_new_u64(std::uint64_t value);

/// Create float values
GblnValue* gbln_value_new_f32(float value);
GblnValue* gbln_value_new_f64(double value);

/// Create string value
GblnValue* gbln_value_new_str(const char* value, std::size_t max_len);

/// Create boolean value
GblnValue* gbln_value_new_bool(bool value);

/// Create null value
GblnValue* gbln_value_new_null();

/// Create empty object
GblnValue* gbln_value_new_object();

/// Create empty array
GblnValue* gbln_value_new_array();

/* ========================================================================== */
/* Object/Array Building                                                      */
/* ========================================================================== */

/// Insert into object (takes ownership on success)
GblnErrorCode gbln_object_insert(
    GblnValue* object,
    const char* key,
    GblnValue* value
);

/// Push to array (takes ownership on success)
GblnErrorCode gbln_array_push(GblnValue* array, GblnValue* value);

/* ========================================================================== */
/* I/O Operations                                                             */
/* ========================================================================== */

/// Write I/O format file
GblnErrorCode gbln_write_io(
    const GblnValue* value,
    const char* path,
    const GblnConfig* config
);

/// Read I/O format file
GblnErrorCode gbln_read_io(const char* path, GblnValue** out_value);

/* ========================================================================== */
/* Configuration                                                              */
/* ========================================================================== */

/// Create I/O format config
GblnConfig* gbln_config_new_io();

/// Create source format config
GblnConfig* gbln_config_new_source();

/// Create custom config
GblnConfig* gbln_config_new(
    bool mini_mode,
    bool compress,
    std::uint8_t compression_level,
    std::size_t indent,
    bool strip_comments
);

/// Free config
void gbln_config_free(GblnConfig* config);

/// Config getters
bool gbln_config_get_mini_mode(const GblnConfig* config);
bool gbln_config_get_compress(const GblnConfig* config);
std::uint8_t gbln_config_get_compression_level(const GblnConfig* config);
std::size_t gbln_config_get_indent(const GblnConfig* config);
bool gbln_config_get_strip_comments(const GblnConfig* config);

/// Config setters
void gbln_config_set_mini_mode(GblnConfig* config, bool value);
void gbln_config_set_compress(GblnConfig* config, bool value);
void gbln_config_set_compression_level(GblnConfig* config, std::uint8_t value);
void gbln_config_set_indent(GblnConfig* config, std::size_t value);
void gbln_config_set_strip_comments(GblnConfig* config, bool value);

} // extern "C"
