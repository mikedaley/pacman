# Pacman

A classic Pacman game clone built with modern C++20 and SDL3.

## Features

- Classic Pacman gameplay
- Cross-platform support (Windows, macOS, Linux)
- Modern C++20 codebase
- SDL3 for graphics, audio, and input

## Requirements

- CMake 3.25 or higher
- C++20 compatible compiler:
    - GCC 11+
    - Clang 14+
    - MSVC 2022+
- Git (for fetching dependencies)

All other dependencies (SDL3, SDL3_image, SDL3_ttf, SDL3_mixer) are automatically downloaded and built via CMake FetchContent.

## Building

### macOS

```bash
# Install build tools (if not already installed)
xcode-select --install

# Install CMake via Homebrew
brew install cmake
```

#### Release Build

```bash
# Configure
cmake --preset macos-release

# Build
cmake --build build/macos-release

# Run the game
./build/macos-release/pacman.app/Contents/MacOS/pacman
```

#### Debug Build

```bash
# Configure
cmake --preset macos-debug

# Build
cmake --build build/macos-debug

# Run the game
./build/macos-debug/pacman.app/Contents/MacOS/pacman
```

### Linux

```bash
# Install build tools (Debian/Ubuntu)
sudo apt update
sudo apt install build-essential cmake git

# Install SDL3 build dependencies
sudo apt install libx11-dev libxext-dev libxrandr-dev libxcursor-dev \
    libxi-dev libxss-dev libwayland-dev libxkbcommon-dev libegl-dev \
    libdrm-dev libgbm-dev libpulse-dev libasound2-dev libpipewire-0.3-dev
```

#### Release Build

```bash
cmake --preset linux-release
cmake --build build/linux-release

./build/linux-release/pacman
```

#### Debug Build

```bash
cmake --preset linux-debug
cmake --build build/linux-debug

./build/linux-debug/pacman
```

For Fedora/RHEL:

```bash
# Install build tools
sudo dnf install gcc-c++ cmake git

# Install SDL3 build dependencies
sudo dnf install libX11-devel libXext-devel libXrandr-devel libXcursor-devel \
    libXi-devel libXScrnSaver-devel wayland-devel libxkbcommon-devel \
    mesa-libEGL-devel libdrm-devel mesa-libgbm-devel pulseaudio-libs-devel \
    alsa-lib-devel pipewire-devel
```

### Windows

#### Using Visual Studio 2022

```powershell
# Configure (generates Visual Studio solution)
cmake --preset windows-vs2022

# Build Release
cmake --build build/windows-vs2022 --config Release

# Build Debug
cmake --build build/windows-vs2022 --config Debug

# Run the game
.\build\windows-vs2022\Release\pacman.exe
.\build\windows-vs2022\Debug\pacman.exe
```

#### Using Ninja (MSVC)

```powershell
# Release
cmake --preset windows-release
cmake --build build/windows-release

# Debug
cmake --preset windows-debug
cmake --build build/windows-debug
```

## CMake Presets

The project includes CMake presets for each platform and build type:

| Preset            | Description                            |
| ----------------- | -------------------------------------- |
| `macos-debug`     | macOS debug build                      |
| `macos-release`   | macOS release build with optimizations |
| `linux-debug`     | Linux debug build                      |
| `linux-release`   | Linux release build with optimizations |
| `windows-debug`   | Windows debug build (Ninja + MSVC)     |
| `windows-release` | Windows release build (Ninja + MSVC)   |
| `windows-vs2022`  | Visual Studio 2022 solution            |

List available presets:

```bash
cmake --list-presets
```

## IDE Setup

### Zed

The project includes Zed editor configuration in `.zed/`. Open the project folder in Zed and use the task runner (`cmd+shift+t`) to access build tasks:

- **Configure (Debug/Release)** - Run CMake configuration
- **Build (Debug/Release)** - Build the project
- **Run (Debug/Release)** - Run the game
- **Clean** - Remove build directories
- **Rebuild (Debug)** - Clean and rebuild

### VS Code

For VS Code, install the CMake Tools extension and select the appropriate preset from the CMake status bar.

### CLion

CLion automatically detects CMake presets. Select the desired preset from the CMake profiles dropdown.

## Project Structure

```
pacman/
├── CMakeLists.txt          # Main CMake configuration
├── CMakePresets.json       # CMake presets for different build configs
├── .zed/                   # Zed editor configuration
│   ├── settings.json       # LSP settings (clangd)
│   └── tasks.json          # Build/run tasks
├── src/
│   ├── main.cpp            # Application entry point
│   ├── core/
│   │   ├── Game.hpp/cpp    # Main game loop and state
│   │   ├── Window.hpp/cpp  # SDL window wrapper
│   │   └── Renderer.hpp/cpp# SDL renderer wrapper
│   └── utils/
│       ├── Types.hpp       # Type aliases and common types
│       └── Constants.hpp   # Game constants and settings
├── assets/
│   ├── audio/              # Sound effects and music
│   ├── fonts/              # TTF fonts for text rendering
│   ├── maps/               # Level/maze data files
│   └── sprites/            # Game sprites and textures
└── build/                  # Build output (generated)
```

## Controls

| Key        | Action      |
| ---------- | ----------- |
| Arrow Keys | Move Pacman |
| ESC        | Quit game   |

## Debugging

### macOS / Linux

Use LLDB or GDB with the debug build:

```bash
# macOS
lldb ./build/macos-debug/pacman.app/Contents/MacOS/pacman

# Linux
gdb ./build/linux-debug/pacman
```

### Windows

Open the Visual Studio solution or use Visual Studio's debugger with the debug build.

## License

This project is for educational purposes.
