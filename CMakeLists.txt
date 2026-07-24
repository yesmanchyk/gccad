cmake_minimum_required(VERSION 3.20)
project(gccad CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags for C++26 reflection
add_compile_options(-freflection)

# 1. Target for the reflection-based generator binary
add_executable(generator main.cpp)

# 2. Custom command: run generator to generate derivatives.h whenever main.cpp, functions.h, or differentiator.h change
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/derivatives.h
    COMMAND generator
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS generator functions.h differentiator.h
    COMMENT "Generating derivatives.h via reflection generator"
)

# 3. Target for test_derivatives executable, depending on generated derivatives.h
add_executable(test_derivatives test_derivatives.cpp ${CMAKE_CURRENT_BINARY_DIR}/derivatives.h)

# Ensure header search path includes source dir and binary build dir for derivatives.h
target_include_directories(test_derivatives PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
