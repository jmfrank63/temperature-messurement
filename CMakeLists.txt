cmake_minimum_required(VERSION 3.10)

project(temperature_measurement LANGUAGES CXX)

# Set the install prefix to the build directory (or its "install" subdirectory)
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install path" FORCE)

# Default to Debug build if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Use clang++ as the compiler
set(CMAKE_CXX_COMPILER clang++)

# Configure separate flags for debug and release
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -Wall -Wextra -Wpedantic -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -Wpedantic -O3")

# Create the main executable
add_executable(temperature_measurement
    src/main.cpp
    src/config.cpp
    # ...add other .cpp files as needed...
)

# Make sure the compiler can find headers in the 'include' folder
target_include_directories(temperature_measurement PRIVATE include)

# Custom target to run Valgrind
add_custom_target(run_valgrind
    COMMAND valgrind --leak-check=full --show-leak-kinds=all ./temperature_measurement
    DEPENDS temperature_measurement
)

# Enable testing and add Google Test
enable_testing()
find_package(GTest REQUIRED)

# Create the test executable
add_executable(temperature_tests
    tests/test_temperature_generator.cpp
    tests/test_ring_buffer.cpp
    src/config.cpp
)
target_include_directories(temperature_tests PRIVATE include)
target_link_libraries(temperature_tests PRIVATE GTest::gtest_main)

# Register the test
add_test(NAME temperature_tests COMMAND temperature_tests)

# Installation rules for the main executable
install(TARGETS temperature_measurement
    RUNTIME DESTINATION bin
)

# Installation rules for the main executable
install(TARGETS temperature_measurement
    RUNTIME DESTINATION bin
)

# Installation rules for configuration files
install(DIRECTORY config
    DESTINATION share/temperature_measurement
)
