# Qt 6 requires at least CMake 3.16
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(QtClientSideWindowDecoration VERSION 0.1 LANGUAGES CXX)
message(STATUS "QtClientSideWindowDecoration Version: ${PROJECT_VERSION}")

# Add option to prevent example from building, only build it by default if
# this is not used as a subproject
if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(_build_example_default YES)
else ()
    set(_build_example_default NO)
endif ()
option(BUILD_EXAMPLE "Build an example application." ${_build_example_default})
# Installing only installs the library unless the example is requested
option(INSTALL_EXAMPLE "Install the example application." NO)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Define C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Let installed executables find the library next to them
if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")
else ()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif ()

# Include Qt6 as an imported target
find_package(Qt6 6.4 COMPONENTS Widgets Svg REQUIRED)
message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "Qt6 Directory: ${Qt6_DIR}")

# Build library
add_subdirectory(library)

# Build example
if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
