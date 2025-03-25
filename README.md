//// filepath: /home/johannes/Projects/C++/temperature-messurement/README.md
# Temperature Measurement Project

This project demonstrates how to capture a sequence of temperature measurements, store them in a ring buffer, and track the minimum and maximum temperatures in a naive way (updating on write and rescanning the buffer when needed).

## Features

- **Configuration File:**
  Uses a plain text configuration file (`config/temperature_config.txt`) for parameters such as:
  - `baseOffset` (seasonal base temperature)
  - `amplitude` (seasonal temperature swing)
  - `daysInYear` (used for computing the seasonal sine curve)
  - `minTemp` / `maxTemp` (allowed temperature limits)
  - `driftFactor` (how strongly temperatures are pulled toward the seasonal base)
  - `bufferSize` (the number of measurements to retain)

  Example `temperature_config.txt`:
  ```
  baseOffset 15.0
  amplitude 10.0
  daysInYear 365.0
  minTemp -20.0
  maxTemp 50.0
  driftFactor 0.1
  bufferSize 100
  ```

- **Ring Buffer:**
  Implemented as a templated container which supports storing any data type (doubles in production). When full, new writes will automatically overwrite the oldest data. Additionally, the ring buffer exposes the next overwrite candidate via `getOverwriteCandidate()`.

- **Temperature Buffer:**
  A wrapper around the ring buffer that tracks minimum and maximum temperatures.
  - **On push:** The new temperature is added, and if the buffer is not yet full, the min and max are updated directly.
  - **On overwrite:** If the value being overwritten equals the current min or max, the entire buffer is rescanned to determine new extremes.

- **Testing:**
  GoogleTest tests cover:
  - Basic functionality of the RingBuffer (for both `int` and `double` data types).
  - All “edge case” scenarios: empty buffer, partial fill, overflow (multiple wraps), single-element buffer, and repeated values.
  - TemperatureBuffer tests verify that min and max tracking work correctly even when overwriting the min or max value, when all values are equal, and for a mix of negative and positive temperatures.

## Build and Run

### Requirements

- CMake 3.10 or later
- clang++ compiler
- Valgrind (optional, for memory checking)
- GoogleTest (GTest)

### Building

1. **Configure and Build in Debug mode:**
    ```bash
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    make
    ```

2. **For a Release Build (with aggressive optimization -O3):**
    ```bash
    rm -rf build
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    ```

### Testing

From your build directory, run:
```bash
ctest --verbose
```
To list all registered tests (including the RingBuffer tests for doubles and TemperatureBuffer tests), execute:
```bash
./temperature_tests --gtest_list_tests
```

### Installation

The default install prefix is set to the build directory. To install the main executable and configuration files:
```bash
make install
```
This will install the executable to `build/install/bin` and the configuration files to `build/install/share/temperature_measurement/config`.

## Code Structure

```
temperature-messurement/
├── CMakeLists.txt      # Build configuration file
├── README.md           # Project documentation (this file)
├── config/
│   └── temperature_config.txt   # Configuration file with defaults
├── include/
│   ├── defaults.h      # Default constants for configuration
│   ├── config.h        # Configuration structure and loader prototype
│   ├── ring_buffer.h   # Templated ring buffer implementation
│   └── temperature_buffer.h   # Temperature buffer implementation updating min/max
├── src/
│   ├── main.cpp        # Main application using TemperatureBuffer and config loader
│   └── config.cpp      # Implementation of configuration loader
└── tests/
    ├── test_ring_buffer.cpp    # Tests verifying ring buffer functionality (with ints and doubles)
    └── test_temperature_buffer.cpp   # Tests verifying TemperatureBuffer's min/max tracking
```

## Usage Example

The `main.cpp` simulates 100 days of temperature measurements. For each day, it:
- Computes the seasonal base temperature using a sine function.
- Adds a random walk step.
- Clamps the result between `minTemp` and `maxTemp`.
- Pushes the measurement into the `TemperatureBuffer`.

Finally, it prints the number of stored measurements along with the current minimum and maximum temperatures.

## Summary

This project demonstrates:
- Reading a configuration file with defaults.
- Storing measurements in a ring buffer.
- Naively tracking min/max via a TemperatureBuffer that updates on each write and rescans when needed.
- Comprehensive testing of edge cases for both the ring buffer and temperature buffer.
