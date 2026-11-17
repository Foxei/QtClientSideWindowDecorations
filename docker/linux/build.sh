#!/usr/bin/env bash
# Builds the project for Linux and installs it into /out.
set -euo pipefail

SRC=/src
BUILD=/build/linux
OUT=/out

cmake -S "$SRC" -B "$BUILD" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DINSTALL_EXAMPLE=ON \
    -DCMAKE_INSTALL_PREFIX="$OUT"
cmake --build "$BUILD" --parallel
rm -rf "${OUT:?}"/*
cmake --install "$BUILD"

echo "Linux build installed to $OUT"
