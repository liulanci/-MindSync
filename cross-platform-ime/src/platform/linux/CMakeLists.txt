cmake_minimum_required(VERSION 3.15)

find_package(PkgConfig REQUIRED)
pkg_check_modules(IBUS REQUIRED ibus-1.0)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(X11 REQUIRED x11)

set(LINUX_SOURCES
    ibus_adapter/ibus_platform_adapter.cpp
)

add_library(ime_platform_linux ${LINUX_SOURCES})

target_include_directories(ime_platform_linux PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${IBUS_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
)

target_link_libraries(ime_platform_linux PUBLIC
    ime_core
    ${IBUS_LIBRARIES}
    ${GTK3_LIBRARIES}
    ${X11_LIBRARIES}
)

set_target_properties(ime_platform_linux PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)