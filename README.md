# Tiny Software Rasterizer

**Tiny Software Rasterizer** is a implementation of the hierarchy z-buffer algorithm.

<img src="./image/screenshot.png" alt="screenshot" style="zoom: 50%;" />

## Prerequisites

To build the project, you must first install the following tools.

### macOS >= 10.15 (Intel or Apple Silicon)
- Xcode 12.3 (or more recent)
- CMake 3.19 (or more recent)

## Build and Run

### macOS

To compile the project, you must have the most recent version of Xcode installed.
Then run 'cmake' from the project's root directory, to generate a project of Xcode.

```shell
cmake -S . -B build -G "Xcode"
```
Now, you can open the Xcode project in `build` folder and play the program with IDE, or only build and run the project with the following commands.
```sh
cmake --build build --config Release
cd bin
./Editor
```

> **NOTICE**
>
> There is a huge performance difference between using Debug mode and Release mode to compile programs, so be sure to use Release mode for non-debugging purposes.
