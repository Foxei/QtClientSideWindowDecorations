#!/usr/bin/env bash
# Cross compiles the project for macOS (universal arm64 + x86_64) and creates a
# self contained, ad-hoc signed application bundle in /out.
set -euo pipefail

SRC=/src
BUILD=/build/macos
OUT=/out
STAGE=/build/macos-install
APP="$STAGE/Example Application.app"
QT=/opt/qt/macos
TARGET_PREFIX=$(ls /opt/osxcross/bin/arm64-apple-darwin*-otool | sed 's/-otool$//')
OTOOL="$TARGET_PREFIX-otool"
INSTALL_NAME_TOOL="$TARGET_PREFIX-install_name_tool"

cmake -S "$SRC" -B "$BUILD" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$SRC/docker/macos/toolchain.cmake" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$STAGE"
cmake --build "$BUILD" --parallel
rm -rf "$STAGE"
cmake --install "$BUILD"

# Move the decoration library into the bundle
mkdir -p "$APP/Contents/Frameworks" "$APP/Contents/PlugIns" "$APP/Contents/Resources"
mv "$STAGE"/lib/libclient_side_window_decoration*.dylib "$APP/Contents/Frameworks/"

# Qt plugins required at runtime. They look for frameworks in ../../lib by
# default, so add an rpath pointing to the bundled frameworks.
for plugin in platforms/libqcocoa.dylib styles/libqmacstyle.dylib \
              imageformats/libqsvg.dylib iconengines/libqsvgicon.dylib; do
    mkdir -p "$APP/Contents/PlugIns/$(dirname "$plugin")"
    cp "$QT/plugins/$plugin" "$APP/Contents/PlugIns/$plugin"
    # The invalidated signature is restored by signing the bundle below
    "$INSTALL_NAME_TOOL" -add_rpath @loader_path/../../Frameworks "$APP/Contents/PlugIns/$plugin" \
        2> >(grep -v "invalidate the code signature" >&2)
done
printf '[Paths]\nPlugins = PlugIns\n' > "$APP/Contents/Resources/qt.conf"

# Recursively copy every Qt framework referenced via @rpath
copy_deps() {
    local binary=$1 framework
    for framework in $("$OTOOL" -L "$binary" | awk '/@rpath\/Qt[A-Za-z]*\.framework/ {print $1}' \
                       | sed -E 's#@rpath/([^/]+\.framework)/.*#\1#' | sort -u); do
        [ -d "$APP/Contents/Frameworks/$framework" ] && continue
        cp -R "$QT/lib/$framework" "$APP/Contents/Frameworks/"
        rm -rf "$APP/Contents/Frameworks/$framework/Headers" \
               "$APP/Contents/Frameworks/$framework/Versions/A/Headers"
        copy_deps "$APP/Contents/Frameworks/$framework/${framework%.framework}"
    done
}
while IFS= read -r -d '' binary; do
    copy_deps "$binary"
done < <(find "$APP/Contents/MacOS" "$APP/Contents/Frameworks" "$APP/Contents/PlugIns" -type f \
              \( -perm -u+x -o -name '*.dylib' \) -print0)

# Ad-hoc sign the bundle, modified binaries are not runnable on Apple Silicon otherwise
rcodesign sign "$APP"

rm -rf "${OUT:?}"/*
cp -R "$APP" "$OUT/"
(cd "$STAGE" && tar -czf "$OUT/ExampleApplication-macos-universal.tar.gz" "Example Application.app")

echo "macOS build installed to $OUT"
