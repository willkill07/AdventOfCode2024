# Advent Of Code 2024

Tested on Linux with libstdc++ >= 14.1 and clang >= 18.1

## Prerequisites

1. CMake (version >= 3.28)
2. libstdc++ >= 14.1
3. clang >= 18.1
3. Ninja

## Building

```
CXX=clang++ cmake -B build -GNinja && cmake --build build
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
