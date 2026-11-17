# Enable Qt6 cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Make cmake to find own .ui file
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui/)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(FRAMEWORK_RES frameworks/logo.icns)

# Define an executable
add_executable(example_application
        resources/application_resources.qrc
        src/main.cpp
        src/MainWindow.cpp
        ${FRAMEWORK_RES})

target_link_libraries(example_application PRIVATE Qt6::Widgets Qt6::Svg client_side_window_decoration)

# Setting platform specific post build options
if (APPLE)
    # Setting application meta information
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE TRUE)
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Example Application")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_EXECUTABLE_NAME "Example Application")
    set_target_properties(example_application PROPERTIES OUTPUT_NAME "Example Application")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_INFO_STRING "Example Application - Version ${PROJECT_VERSION}")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "Simon Schäfer")
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_COPYRIGHT "Copyright © 2022 Simon Schäfer. All rights reserved.")
    #
    set_source_files_properties(${FRAMEWORK_RES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/)
    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_ICON_FILE "logo.icns")

    set_target_properties(example_application PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/frameworks/MacOSXBundleInfo.plist.in")
elseif (WIN32)
    # Do not open a console window next to the application
    set_target_properties(example_application PROPERTIES WIN32_EXECUTABLE TRUE)
elseif (UNIX)
    # Setting application meta information
    set(PROJECT_PATH "${CMAKE_CURRENT_BINARY_DIR}/example_application")
    set(PROJECT_ICON "${CMAKE_CURRENT_BINARY_DIR}/logo.png")

    # Copy launcher files
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/logo.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/frameworks/linux-launcher.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/linux-launcher.desktop)
endif ()

# Install the application next to the library
install(TARGETS example_application
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
