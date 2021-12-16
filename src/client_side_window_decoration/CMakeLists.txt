# Define library. Only source files here!
project(libclient_side_window_decoration VERSION 0.1 LANGUAGES CXX)

# Enable Qt5 cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui/)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set platform specific sources
if (APPLE)
    set(PLATFORM_SOURCE src/OSXHideTitleBar.mm)
    set(FRAMEWORK_RES ${CMAKE_SOURCE_DIR}/frameworks/logo.icns)
elseif (UNIX)
    set(PLATFORM_SOURCE)
    set(FRAMEWORK_RES)
elseif (WIN32)
    set(PLATFORM_SOURCE src/WinNativeWindow.cpp src/QWinWidget.cpp)
    set(FRAMEWORK_RES)
endif ()

add_library(client_side_window_decoration SHARED ${PLATFORM_SOURCE} ${FRAMEWORK_RES}
        resources/decoration_resources.qrc
        include/client_side_window_decoration/UndecoratedWindow.h
        src/UndecoratedWindow.cpp)

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(client_side_window_decoration PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# Set platform specific linker options
if (APPLE)
    target_link_libraries(client_side_window_decoration PRIVATE Qt5::Widgets Qt5::Svg)
elseif (UNIX)
    target_link_libraries(client_side_window_decoration PRIVATE Qt5::Widgets Qt5::Svg)
elseif (WIN32)
    target_link_libraries(client_side_window_decoration PRIVATE Qt5::Widgets Qt5::Svg Dwmapi.lib)
endif ()

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS client_side_window_decoration EXPORT ClientSideWindowDecorationConfig
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT ClientSideWindowDecorationConfig DESTINATION share/ClientSideWindowDecoration/cmake)

# This makes the project importable from the build directory
export(TARGETS client_side_window_decoration FILE ClientSideWindowDecorationConfig.cmake)

if (APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "-framework ServiceManagement -framework Foundation -framework Cocoa")
endif ()