cmake_minimum_required(VERSION 3.15)

set(WINDOWS_SOURCES
    tsf_adapter/tsf_platform_adapter.cpp
)

add_library(ime_platform_windows ${WINDOWS_SOURCES})

target_include_directories(ime_platform_windows PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ime_platform_windows PUBLIC
    ime_core
    msctf.lib
    imm32.lib
)

set_target_properties(ime_platform_windows PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)