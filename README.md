# Temperature Measurement Project

This project generates a sequence of temperature measurements using a random walk approach with a Gaussian-like distribution. Smaller steps (+1/-1) occur frequently, while large jumps (+20/-20) are rare. Approximately 1 in 1000 measurements may yield a ±3 deviation, with ±1 dominating around 63% of the time.

## Features

- **Memory Storage**
  Measurements are written into a ring buffer for efficient use of memory. Only the most recent N measurements are kept.

- **Min/Max Tracking**
  Two separate linked lists maintain minimum and maximum values over the stored measurements.

- **Multi-threaded File Write**
  Measurements are periodically saved to a file in a separate thread to avoid blocking the main application.

## Project Structure

- `src/`
  Implementation files (.cpp) for the ring buffer, linked lists, and multi-threaded file saving.

- `include/`
  Header files (.h/.hpp) declaring data structures and function interfaces.

- `tests/`
  Unit tests for verifying correctness of the ring buffer, random walk generator, and min/max tracking.

- `CMakeLists.txt`
  Build configuration using CMake.

## Building and Running

1. Navigate to the project directory.
2. Create a build folder and run CMake.
3. Build the executable.
4. Run the application.

```bash
mkdir build && cd build
cmake ..
make
./temperature_measurement
```

## License

Specify your chosen license here if required.
