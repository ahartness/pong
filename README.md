# Pong with raylib

Small C++ project for building a **Pong** game with [raylib](https://www.raylib.com/).

## Current status

The project now opens a Pong window with two visible paddles and a moving ball. The player paddle is controlled with **W/S** or the **Up/Down** arrow keys, while the AI paddle is currently drawn in place without behavior yet.

## Requirements

- CMake 3.16+
- C++17 compiler
- raylib installed and discoverable by CMake

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/pong
```

## Project structure

```text
.
├── CMakeLists.txt
├── README.md
└── src/
    └── main.cpp
```

## Turning this into Pong

Typical next steps:

1. Add AI movement for the second paddle.
2. Track score for each player.
3. Add a reset condition after each point.

## Notes

On Linux, raylib may require extra system libraries. This project already links the common Linux dependencies in `CMakeLists.txt`.