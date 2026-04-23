cmake_minimum_required(VERSION 3.15)

find_library(INPUT_METHOD_KIT InputMethodKit)
find_library(APPKIT AppKit)

set(MACOS_SOURCES
    input_method_kit/imk_platform_adapter.mm
)

add_library(ime_platform_macos ${MACOS_SOURCES})

target_include_directories(ime_platform_macos PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ime_platform_macos PUBLIC
    ime_core
    ${INPUT_METHOD_KIT}
    ${APPKIT}
)

set_target_properties(ime_platform_macos PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)