# Qt Client Side Window Decorations

[![Build](https://github.com/Foxei/QtClientSideWindowDecorations/actions/workflows/build.yml/badge.svg)](https://github.com/Foxei/QtClientSideWindowDecorations/actions/workflows/build.yml)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](COPYING.LESSER)

A small Qt 6 library for client side window decorations: draw your own title bar while keeping the native
window behaviour of every desktop platform. Previous approaches often break Aero Snap on Windows or resizing on
macOS. This library keeps them working and allows identical theming across all platforms.

## Features

* Tiling, snapping and fullscreen ✅
* Save and restore window states ✅
* Resize ✅
* Move ✅

## Preview

![macOS Screenshot](documentation/images/MacOs.png)
![Windows Screenshot](documentation/images/Windows.png)
![Ubuntu Screenshot](documentation/images/Ubuntu.png)

## Requirements

* Qt 6.4 or newer (Widgets and Svg)
* CMake 3.16 or newer
* A C++17 compiler

## Build

### With Docker (all platforms)

All platforms are built inside containers, nothing besides Docker needs to be installed on the host.

```bash
docker compose up --build
```

The results end up in `dist/`:

| Platform | Toolchain                                                                                                     | Output                                                                  |
|----------|---------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------|
| Linux    | Ubuntu 24.04, system Qt 6                                                                                      | `dist/linux` (install tree)                                             |
| Windows  | Fedora MinGW-w64 cross compiler and Qt 6 (x86_64)                                                              | `dist/windows/ExampleApplication` + zip with all DLLs                   |
| macOS    | [osxcross](https://github.com/tpoechtrager/osxcross) and the official Qt for macOS (universal arm64 + x86_64) | `dist/macos/Example Application.app` + tar.gz, ad-hoc signed            |

Single platforms can be built with e.g. `docker compose run --rm windows`.

The macOS SDK cannot be redistributed and is therefore taken from the host. By default the SDK of the
Xcode command line tools (`/Library/Developer/CommandLineTools/SDKs/MacOSX15.4.sdk`) is used. On other
hosts point `MACOS_SDK_PATH` to an unpacked `MacOSX<version>.sdk` directory and set `MACOS_SDK_VERSION`
accordingly, for example in a `.env` file:

```bash
MACOS_SDK_PATH=/path/to/MacOSX15.4.sdk
MACOS_SDK_VERSION=15.4
```

### Dev Container

The repository contains a [Dev Container](https://containers.dev) (`.devcontainer/`) based on the Linux build image with
CMake, Ninja, clangd and gdb. Open the folder in VS Code and select *Reopen in Container*.

### Native

```bash
cmake -S . -B build
cmake --build build
```

### Continuous Integration

Every push and pull request is built by GitHub Actions for all three platforms. Linux and Windows use the same
Docker Compose services as above, macOS is built natively on a macOS runner. Pushing a tag starting with `v`
(e.g. `v1.0.0`) creates a GitHub release with the packaged example application for every platform.

## Use as Library

### Install

Build and install the library system wide (default prefix `/usr/local` on Linux and macOS):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLE=OFF
cmake --build build
sudo cmake --install build
```

Use `-DCMAKE_INSTALL_PREFIX=<path>` to install somewhere else. This installs the shared library, the headers and a
CMake package configuration. The example application is only installed with `-DINSTALL_EXAMPLE=ON`.

| Option            | Default                     | Description                        |
|-------------------|-----------------------------|------------------------------------|
| `BUILD_EXAMPLE`   | `ON` (`OFF` as subproject)  | Build the example application.     |
| `INSTALL_EXAMPLE` | `OFF`                       | Install the example application.   |

### Add to your project

After installing, import the library in the `CMakeLists.txt` of your application:

```cmake
find_package(ClientSideWindowDecoration 0.1 REQUIRED)
target_link_libraries(my_app PRIVATE ClientSideWindowDecoration::client_side_window_decoration)
```

Qt 6 Widgets is found automatically as a dependency. If the library was installed to a custom prefix, pass
`-DCMAKE_PREFIX_PATH=<prefix>` when configuring your application. A minimal example application is located in
[`tests/install`](tests/install), `docker compose run --rm linux /src/docker/linux/test-install.sh` installs the
library inside a container and builds and runs it.

Alternatively add the repository as a subdirectory (the example is not built in that case):

```cmake
add_subdirectory(QtClientSideWindowDecorations)
target_link_libraries(my_app PRIVATE ClientSideWindowDecoration::client_side_window_decoration)
```

### Usage

Derive your main window from `UndecoratedWindow` instead of `QMainWindow` and add your widgets to the central
widget. Do not load a second `QMainWindow` from a ui file.

```cpp
#include <client_side_window_decoration/UndecoratedWindow.h>

class MainWindow : public UndecoratedWindow {
  Q_OBJECT
 public:
  explicit MainWindow(QWidget *parent = nullptr) : UndecoratedWindow(parent) {
    centralWidget()->layout()->addWidget(new QLabel("Hello World"));
  }
};
```

The title bar controls can be placed on the left or the right side with `setTitleBarDirection()`.

### Platform capabilities

Not every feature is available on every platform. The library exposes compile time flags to check this:

| Flag                                                | macOS   | Windows | Linux  | Description                                                                     |
|-----------------------------------------------------|---------|---------|--------|---------------------------------------------------------------------------------|
| `UndecoratedWindow::SUPPORTS_RIGHT_SIDE_CONTROLS`   | `false` | `true`  | `true` | Window controls can be placed on the right side (`Qt::LeftToRight` direction). |

```cpp
if constexpr (UndecoratedWindow::SUPPORTS_RIGHT_SIDE_CONTROLS) {
  window->setTitleBarDirection(Qt::LeftToRight);
}
```

## Implementation Details

### macOS
Native Objective-C code and the Cocoa API hide the title bar. The calls are repeated for every window state change.
The native window controls are kept and the background of the controls is placed behind them.

### Windows
The QWindow is embedded in a native Windows window. Corresponding system calls from the window decoration are forwarded.

### Linux
The `startSystemResize` and `startSystemMove` functions of Qt are used together with a `Qt::FramelessWindowHint` window.

## License

This library is licensed under the [GNU Lesser General Public License v3.0](COPYING.LESSER), which supplements the
[GNU General Public License v3.0](COPYING). You can use it in proprietary applications as long as changes to the
library itself are published under the same license and users can replace the library (e.g. by linking dynamically).
