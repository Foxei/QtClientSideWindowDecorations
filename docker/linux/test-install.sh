#!/usr/bin/env bash
# Installs the library system wide (default prefix /usr/local) inside the
# container and builds and runs an external application that finds it with
# find_package, without any additional search paths.
set -euo pipefail

SRC=/src
BUILD=/tmp/install-test

cmake -S "$SRC" -B "$BUILD/library" -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLE=OFF
cmake --build "$BUILD/library" --parallel
cmake --install "$BUILD/library"
ldconfig

cmake -S "$SRC/tests/install" -B "$BUILD/consumer" -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD/consumer" --parallel
QT_QPA_PLATFORM=offscreen "$BUILD/consumer/install_test"

echo "Install test passed"
