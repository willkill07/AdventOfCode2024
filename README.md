# Advent Of Code 2024

Tested on Linux and macOS with GCC >= 14.1

## Building

```
cmake -B build && cmake --build build
```

To specify GCC >= 14.1 on macOS, add the following CMake definition (assuming homebrew):
```
cmake -B build -G Ninja -DCMAKE_CXX_COMPILER=g++-14 && cmake --build build
```

## Running

```
./build/AdventOfCode2024
```

## Input File Expectations

1. Create a `inputs` directory:

   ```bash
   mkdir inputs
   ```

2. Naming should adhere to `dayXX.txt` where `XX` is a zero-padded number representing the day. For example, `inputs/day01.txt` will be opened when running Day 01. `inputs/day20.txt` will be opened when running Day 20.
