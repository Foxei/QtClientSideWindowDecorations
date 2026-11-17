#!/usr/bin/env bash
# Cross compiles the project for Windows (x86_64) and creates a self contained
# folder (plus zip) with all required DLLs and Qt plugins in /out.
set -euo pipefail

SRC=/src
BUILD=/build/windows
OUT=/out
STAGE=/build/windows-install/ExampleApplication
SYSROOT=/usr/x86_64-w64-mingw32/sys-root/mingw
OBJDUMP=x86_64-w64-mingw32-objdump

mingw64-cmake -S "$SRC" -B "$BUILD" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$STAGE" \
    -DCMAKE_INSTALL_BINDIR=. \
    -DCMAKE_INSTALL_LIBDIR=lib
cmake --build "$BUILD" --parallel
rm -rf "$STAGE"
cmake --install "$BUILD"

# Qt plugins required at runtime
QT_PLUGINS="$SYSROOT/lib/qt6/plugins"
for plugin in platforms/qwindows.dll styles/qmodernwindowsstyle.dll \
              imageformats/qsvg.dll iconengines/qsvgicon.dll; do
    if [ -f "$QT_PLUGINS/$plugin" ]; then
        mkdir -p "$STAGE/$(dirname "$plugin")"
        cp "$QT_PLUGINS/$plugin" "$STAGE/$plugin"
    fi
done

# Recursively copy every DLL dependency that is shipped by the MinGW sysroot
copy_deps() {
    local binary=$1 dll
    for dll in $("$OBJDUMP" -p "$binary" | awk '/DLL Name:/ {print $3}'); do
        [ -f "$STAGE/$dll" ] && continue
        if [ -f "$SYSROOT/bin/$dll" ]; then
            cp "$SYSROOT/bin/$dll" "$STAGE/$dll"
            copy_deps "$STAGE/$dll"
        fi
    done
}
while IFS= read -r -d '' binary; do
    copy_deps "$binary"
done < <(find "$STAGE" -name '*.exe' -print0 -o -name '*.dll' -print0)

rm -rf "${OUT:?}"/*
cp -r "$STAGE" "$OUT/"
(cd "$(dirname "$STAGE")" && zip -qr "$OUT/ExampleApplication-windows-x64.zip" "$(basename "$STAGE")")

echo "Windows build installed to $OUT"
