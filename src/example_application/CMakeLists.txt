# Enable Qt5 cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui/)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Define an executable
add_executable(example_application
        resources/application_resources.qrc
        src/main.cpp
        src/MainWindow.cpp)

if (APPLE)
    target_link_libraries(example_application Qt5::Widgets Qt5::Svg client_side_window_decoration)
elseif (UNIX)
    target_link_libraries(example_application PRIVATE Qt5::Widgets Qt5::Svg client_side_window_decoration)
elseif (WIN32)
    target_link_libraries(example_application PRIVATE Qt5::Widgets Qt5::Svg client_side_window_decoration)
endif ()

target_include_directories(example_application PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

if (APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-framework ServiceManagement -framework Foundation -framework Cocoa")
    # Setting application meta information
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE TRUE)
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_EXECUTABLE_NAME "${PROJECT_NAME}")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} - Version ${VERSION}")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "Informatik 11 - Embedded Software")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_COPYRIGHT "Copyright © 2021 Simon Schäfer. All rights reserved.")
    #
    set_source_files_properties(${FRAMEWORK_RES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/)
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_ICON_FILE "logo.icns")

    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/frameworks/MacOSXBundleInfo.plist.in")
elseif (UNIX)
    # Fix double click bug
    set(CMAKE_EXE_LINKER_FLAGS -no-pie)

    # Setting application meta information
    set(PROJECT_PATH "${CMAKE_BINARY_DIR}/example_application")
    set(PROJECT_ICON "${CMAKE_BINARY_DIR}/logo.png")

    # Copy launcher files
    file(COPY ${CMAKE_SOURCE_DIR}/frameworks/logo.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    configure_file(${CMAKE_SOURCE_DIR}/frameworks/linux-launcher.desktop.in $linux-launcher.desktop)
endif ()
