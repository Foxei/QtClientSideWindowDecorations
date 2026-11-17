# CMake toolchain for cross compiling to macOS with osxcross.
set(CMAKE_SYSTEM_NAME Darwin)

file(GLOB _osxcross_clang /opt/osxcross/bin/arm64-apple-darwin*-clang)
list(GET _osxcross_clang 0 _osxcross_clang)
string(REGEX REPLACE "-clang$" "" OSXCROSS_TARGET_PREFIX "${_osxcross_clang}")

set(CMAKE_C_COMPILER "${OSXCROSS_TARGET_PREFIX}-clang")
set(CMAKE_CXX_COMPILER "${OSXCROSS_TARGET_PREFIX}-clang++")
set(CMAKE_AR "${OSXCROSS_TARGET_PREFIX}-ar" CACHE FILEPATH "")
set(CMAKE_RANLIB "${OSXCROSS_TARGET_PREFIX}-ranlib" CACHE FILEPATH "")
set(CMAKE_INSTALL_NAME_TOOL "${OSXCROSS_TARGET_PREFIX}-install_name_tool" CACHE FILEPATH "")
set(CMAKE_OTOOL "${OSXCROSS_TARGET_PREFIX}-otool" CACHE FILEPATH "")
set(CMAKE_LIPO "${OSXCROSS_TARGET_PREFIX}-lipo" CACHE FILEPATH "")

file(GLOB CMAKE_OSX_SYSROOT /opt/osxcross/SDK/MacOSX*.sdk)
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "")

# Find target libraries (Qt for macOS) in the target prefix, tools on the host
set(QT_HOST_PATH /opt/qt/host)
set(CMAKE_FIND_ROOT_PATH /opt/qt/macos "${CMAKE_OSX_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Executables are only used from the bundle, link with the final rpath directly
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

# The Linux host rcc supports zstd, the macOS Qt libraries do not
set(CMAKE_AUTORCC_OPTIONS "--no-zstd")
