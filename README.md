# Pong with raylib

Small C++ project for building a **Pong** game with [raylib](https://www.raylib.com/).

## Current status

The project is set up with CMake and raylib, and the current starter code opens a window and draws a simple message. It is ready to be expanded into a full Pong game.

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

1. Add left and right paddles.
2. Add a moving ball with wall and paddle collision.
3. Track score for each player.
4. Add keyboard controls and a reset condition after each point.

## Notes

On Linux, raylib may require extra system libraries. This project already links the common Linux dependencies in `CMakeLists.txt`.