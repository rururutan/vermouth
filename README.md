# Vermouth

A modernized fork of the original Vermouth project.

Vermouth is a Timidity compatible software synthesizer that commonly used with **[hoot](http://dmpsoft.s17.xrea.com/hoot)**.

## Overview

This fork focuses on improving compatibility with modern Windows environments while preserving the original functionality and behavior of Vermouth.

Enhancements include:

* x64 support
* High-DPI awareness
* Dark mode title bar support
* Various maintenance and modernization fixes

The playback engine and application behavior are intended to remain compatible with the original project.

## Features

### x64 Support

The project can be built for both x86 and x64 platforms.

### High-DPI Support

Improved display scaling on modern high-resolution monitors.

### Dark Mode

Supports dark mode title bars on supported versions of Windows.

> Note: Only the title bar is dark-mode aware. The application UI itself is unchanged.

## Requirements

### Runtime

* Windows 10 or later (official binaries)

### Build

* Visual Studio 2022 or later
* CMake

The source code can also be configured to target Windows 7 and later. See the customization section below.

## Building

### Configure

```bash
cmake -S . -B build
cmake -S . -B build_x86 -A Win32
```

### Build

```bash
cmake --build build --config Release
cmake --build build_x86 --config Release
```

## Customization

### Build for Windows 7 or Later

To target Windows 7 and later, modify the following value in `CMakeLists.txt`:

```cpp
WINVER=0x0A00
```

to:

```cpp
WINVER=0x0601
```

### Disable the Key Display Window

Comment out the following line in:

```text
src/Win9x/compiler.h
```

```cpp
#define SUPPORT_KEYDISP
```

## License

This project is distributed under the same license as the original Vermouth project.

BSD 3-Clause License.

See the LICENSE file for details.

## Original Project

Original Vermouth project:

http://retropc.net/yui/hoot/

## Acknowledgements

This project is based on the original Vermouth project. All credit for the original implementation belongs to its original authors and contributors.

