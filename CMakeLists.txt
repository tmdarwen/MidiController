cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME MidiController)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM, C and C++ languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Core/Src/encoder.cpp
    Core/Src/encoders.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# ---------------------------------------------------------------------------
# Host-side unit tests (tests/)
# ---------------------------------------------------------------------------
# tests/ is its own CMake project built with the native host compiler, since
# this project's compiler is pinned to the arm-none-eabi cross-toolchain
# above. Drive it via a nested CMake invocation so `unit_tests` builds and
# runs alongside the firmware whenever the default (ALL) target is built.
# Disable with -DBUILD_UNIT_TESTS=OFF if no host compiler is available.
option(BUILD_UNIT_TESTS "Build and run host-side unit tests as part of the main build" ON)

if(BUILD_UNIT_TESTS)
    add_custom_target(unit_tests ALL
        COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR}/tests -B ${CMAKE_SOURCE_DIR}/tests/build -G Ninja
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/tests/build
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR}/tests/build ctest --output-on-failure
        COMMENT "Building and running host-side unit tests"
        VERBATIM
    )

    # Force unit_tests to complete (and pass) before the firmware is built.
    add_dependencies(${CMAKE_PROJECT_NAME} unit_tests)
endif()
