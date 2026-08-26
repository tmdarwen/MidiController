cmake_minimum_required(VERSION 3.22)
project(MidiControllerTests CXX)

# Host-side unit tests: compiled and run on this machine with the native
# compiler, independent of the ARM cross-toolchain used for the firmware
# build in the parent directory (see ../CMakeLists.txt / ../CMakePresets.json).

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
# Keep googletest's own runtime/build settings self-contained.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(encoder_tests
    encoder_test.cpp
    ../Core/Src/encoder.cpp
)

target_include_directories(encoder_tests PRIVATE
    ../Core/Inc
)

target_link_libraries(encoder_tests PRIVATE
    GTest::gtest_main
)

if(MINGW)
    # Statically link the MinGW runtime (libstdc++, libgcc, winpthread) so the
    # test binary doesn't depend on those DLLs being on PATH at run time.
    target_link_options(encoder_tests PRIVATE -static)
endif()

include(GoogleTest)
gtest_discover_tests(encoder_tests)
