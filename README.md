# Advent Of Code 2024

Tested on Linux and macOS with GCC >= 14.1

CMake will automatically use Homebrew's GCC 14 on macOS.

## Prerequisites

1. CMake (version >= 3.28)
2. GCC >= 14.1
3. Make (or Ninja)

## Building

```
cmake -B build && cmake --build build
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

2. Naming should adhere to `DayXX.txt` where `XX` is a zero-padded number representing the day. For example, `inputs/Day01.txt` will be opened when running Day 01. `inputs/Day20.txt` will be opened when running Day 20.
