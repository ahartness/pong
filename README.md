# Pong with raylib

Small C++ project for building a **Pong** game with [raylib](https://www.raylib.com/).

## Current status

The project now opens a Pong window with two active paddles and a moving ball. The player paddle is controlled with **W/S** or the **Up/Down** arrow keys, while the AI paddle predicts intercepts (including wall bounces) and applies humanized reaction/aim variation.

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

1. Make AI difficulty configurable (speed, reaction delay, and prediction).
2. Add spin/angle control based on where the ball strikes the paddle.
3. Add round flow polish (serve countdown, win condition, and restart UX).

## Notes

On Linux, raylib may require extra system libraries. This project already links the common Linux dependencies in `CMakeLists.txt`.