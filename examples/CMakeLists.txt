# Copyright (c) 2025 Vivian Burkhard Voss
# SPDX-License-Identifier: Apache-2.0

# Basic usage example
add_executable(basic_usage basic_usage.cpp)
target_link_libraries(basic_usage PRIVATE gbln)

# Error handling example
add_executable(error_handling error_handling.cpp)
target_link_libraries(error_handling PRIVATE gbln)

# Install examples (optional)
install(TARGETS basic_usage error_handling
    RUNTIME DESTINATION bin/examples
)
