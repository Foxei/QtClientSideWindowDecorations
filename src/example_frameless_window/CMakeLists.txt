# Enable Qt5 cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui/)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (APPLE)
  set(PLATFORM_SOURCE src/platform/OSXHideTitleBar.mm)
  set(FRAMEWORK_RES ${CMAKE_SOURCE_DIR}/frameworks/logo.icns)
elseif (UNIX)
  set(PLATFORM_SOURCE)
elseif (WIN32)
  set(PLATFORM_SOURCE src/platform/WinNativeWindow.cpp src/platform/QWinWidget.cpp)
endif ()

# Define an executable
add_executable(cpm_dataset_converter ${PLATFORM_SOURCE} ${FRAMEWORK_RES}
    resources/resources.qrc
    src/main.cpp
    src/platform/UndecoratedWindow.cpp
    src/MainWindow.cpp src/platform/NativeDefinitions.h)

if (UNIX)
  target_link_libraries(cpm_dataset_converter PRIVATE
      Qt5::Widgets
      Qt5::Svg)
elseif (WIN32)
  target_link_libraries(cpm_dataset_converter PRIVATE
      Qt5::Widgets
      Qt5::Svg
      Dwmapi.lib)
else ()
  # Define the libraries this project depends upon
  target_link_libraries(cpm_dataset_converter
      Qt5::Widgets
      Qt5::Svg)
endif ()

target_include_directories(cpm_dataset_converter PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

if (APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "-framework ServiceManagement -framework Foundation -framework Cocoa")
  # Setting application meta information
  set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE TRUE)
  set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
  set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_EXECUTABLE_NAME "${PROJECT_NAME}")
  set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} - Version ${VERSION}")
  set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
  set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}")
  set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "Informatik 11 - Embedded Software")
  set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_COPYRIGHT "Copyright © 2021 Simon Schäfer. All rights reserved.")
  #
  set_source_files_properties(${FRAMEWORK_RES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/)
  set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_ICON_FILE "logo.icns")

  set_target_properties(cpm_dataset_converter PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/frameworks/MacOSXBundleInfo.plist.in")
elseif (UNIX)
  # Fix double click bug
  set(CMAKE_EXE_LINKER_FLAGS -no-pie)

  # Setting application meta information
  set(PROJECT_PATH "${CMAKE_BINARY_DIR}/cpm_dataset_converter")
  set(PROJECT_ICON "${CMAKE_BINARY_DIR}/logo.png")

  # Copy launcher files
  file(COPY ${CMAKE_SOURCE_DIR}/frameworks/logo.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
  configure_file(${CMAKE_SOURCE_DIR}/frameworks/linux-launcher.desktop.in $linux-launcher.desktop)
endif ()
