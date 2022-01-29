# At LEAST 3.2 for use of imported targets
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(QtClientSideWindowDecoration VERSION 0.1 LANGUAGES CXX)
message(STATUS "QtClientSideWindowDecoration Version: ${PROJECT_VERSION}")

# Add option to prevent example from building
option(BUILD_EXAMPLE "Build an example application." YES)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Define C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Make sure that for homebrew users qt5 is used instead of 6
if (APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5/")
endif()

# Include Qt5 as an imported target
find_package(Qt5 5.15 COMPONENTS Widgets Svg REQUIRED)
message(STATUS "Qt5 Version: ${Qt5_VERSION}")
message(STATUS "Qt5 Directory: ${Qt5_DIR}")

# Build library
add_subdirectory(library)

# Build example
if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
