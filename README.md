# Temperature Measurement Project

This project demonstrates how to capture a sequence of temperature measurements, store them in a ring buffer, and track the minimum and maximum temperatures. Two min/max tracking implementations are provided:
- A **naive** method that updates min/max on each write and rescans the buffer on overwrite.
- A **buffer** deque-based approach that maintains auxiliary deques for constant-amortized time lookups.

## Features

- **Configuration File:**
  The project uses a plain text configuration file (`config/temperature_config.txt`) for various parameters:
  - `baseOffset` (seasonal base temperature)
  - `amplitude` (seasonal temperature swing)
  - `daysInYear` (used for computing the seasonal sine curve)
  - `minTemp` / `maxTemp` (allowed temperature limits)
  - `driftFactor` (how strongly temperatures are pulled toward the seasonal base)
  - `bufferSize` (the number of measurements to retain)
  - `simulationValues` (the number of temperature measurements to simulate)

  Example `temperature_config.txt`:
  ```
  baseOffset 15.0
  amplitude 10.0
  daysInYear 365.0
  minTemp -20.0
  maxTemp 50.0
  driftFactor 0.1
  bufferSize 10
  simulationValues 20
  ```

- **Default Parameters:**
  Default values for configuration parameters (including the default configuration file path and buffer implementation) are centralized in `defaults.h`. For example, the default config file is set relative to the executable (e.g., `../share/temperature_measurement/config/temperature_config.txt`), and the default buffer implementation is the naive approach.

- **Buffer Implementations:**
  - **Naive Implementation:**
    Uses a TemperatureBuffer that updates min/max on each write and rescans the buffer when the overwritten element is the current extreme.

  - **buffer Implementation:**
    Uses a TemperatureBufferDeque that maintains two deques (for minimum and maximum) so that each update is done in amortized O(1) time.

- **Command Line Parameters:**
  In addition to configuration file parameters, you can control the program behavior via command-line options:
  - **`--config <path>` or `-c <path>`**: Overrides the default configuration file location.
  - **`--buffer` or `-f`**: Uses the buffer (deque-based) min/max tracking implementation.
  - **`--naive` or `-n`**: Uses the naive min/max tracking implementation.

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
To list all registered tests (including tests for the RingBuffer, TemperatureBuffer, and TemperatureBufferDeque implementations), execute:
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
│   ├── defaults.h      # Default constants and default parameter values
│   ├── config.h        # Configuration structure and loader prototype
│   ├── ring_buffer.h   # Templated ring buffer implementation
│   └── temperature_buffer.h       # Naive TemperatureBuffer implementation
│       temperature_buffer_deque.h   # buffer (deque-based) TemperatureBuffer implementation
├── src/
│   ├── main.cpp        # Main application using TemperatureBuffer(s) and config loader
│   └── config.cpp      # Implementation of configuration loader
└── tests/
    ├── test_ring_buffer.cpp         # Tests verifying ring buffer functionality (with ints and doubles)
    └── test_temperature_buffer.cpp  # Tests verifying TemperatureBuffer's min/max tracking
```

## Usage Example

The `main.cpp` simulates `simulationValues` temperature measurements. For each measurement, it:
- Computes the seasonal base temperature using a sine function.
- Adds a random walk step.
- Clamps the result between `minTemp` and `maxTemp`.
- Pushes the measurement into either the naive or buffer TemperatureBuffer (selectable via command-line options).

Command-line examples:
- Default (naive, uses default config)
  ```bash
  ./temperature_measurement
  ```
- Use buffer implementation with an alternative configuration file:
  ```bash
  ./temperature_measurement --buffer --config /path/to/alternative_config.txt
  ```

## Summary

This project demonstrates:
- Reading a configuration file with defaults centralized in `defaults.h`.
- Storing measurements in a templated ring buffer.
- Tracking min/max values in two ways: a naive method and a buffer deque-based method.
- Selection between implementations and configuration override via command-line parameters.
- Comprehensive testing of edge cases for both the ring buffer and temperature buffer implementations.
