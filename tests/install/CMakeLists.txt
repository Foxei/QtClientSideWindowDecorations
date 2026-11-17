# Minimal external application that uses the installed library via find_package
cmake_minimum_required(VERSION 3.16)
project(install_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(ClientSideWindowDecoration REQUIRED)
message(STATUS "ClientSideWindowDecoration Version: ${ClientSideWindowDecoration_VERSION}")

add_executable(install_test main.cpp)
target_link_libraries(install_test PRIVATE ClientSideWindowDecoration::client_side_window_decoration)
