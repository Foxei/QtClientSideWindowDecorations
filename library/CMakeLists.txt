# Define library. Only source files here!
project(libclient_side_window_decoration VERSION 0.1 LANGUAGES CXX)

# Enable Qt6 cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Make library find own .ui file
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui/)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set platform specific sources
if (APPLE)
    set(PLATFORM_SOURCE src/OSXHideTitleBar.mm)
elseif (UNIX)
    set(PLATFORM_SOURCE)
elseif (WIN32)
    set(PLATFORM_SOURCE src/WinNativeWindow.cpp src/QWinWidget.cpp)
endif ()

# Add the library
add_library(client_side_window_decoration SHARED ${PLATFORM_SOURCE}
        resources/decoration_resources.qrc
        include/client_side_window_decoration/UndecoratedWindow.h
        src/UndecoratedWindow.cpp)
add_library(ClientSideWindowDecoration::client_side_window_decoration ALIAS client_side_window_decoration)

# Generate the export macro required for shared libraries on Windows
include(GenerateExportHeader)
generate_export_header(client_side_window_decoration
        EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/client_side_window_decoration/client_side_window_decoration_export.h)

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(client_side_window_decoration PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE src)

# Set platform specific linker options
target_link_libraries(client_side_window_decoration PUBLIC Qt6::Widgets PRIVATE Qt6::Svg)
if (WIN32)
    target_link_libraries(client_side_window_decoration PRIVATE dwmapi)
endif ()

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS client_side_window_decoration EXPORT ClientSideWindowDecorationTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
install(DIRECTORY include/ ${CMAKE_CURRENT_BINARY_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory with
# find_package(ClientSideWindowDecoration)
set(CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/ClientSideWindowDecoration)
install(EXPORT ClientSideWindowDecorationTargets
        NAMESPACE ClientSideWindowDecoration::
        DESTINATION ${CONFIG_INSTALL_DIR})
include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/ClientSideWindowDecorationConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/ClientSideWindowDecorationConfig.cmake
        INSTALL_DESTINATION ${CONFIG_INSTALL_DIR})
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/ClientSideWindowDecorationConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMinorVersion)
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/ClientSideWindowDecorationConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/ClientSideWindowDecorationConfigVersion.cmake
        DESTINATION ${CONFIG_INSTALL_DIR})

# Link cocoa framework on apple to allow for native function calls
if (APPLE)
    target_link_libraries(client_side_window_decoration PRIVATE "-framework Foundation" "-framework Cocoa")
endif ()