cmake_minimum_required(VERSION 3.15)

set(CORE_SOURCES
    src/engine_impl.cpp
    src/input_context.cpp
)

add_library(ime_core ${CORE_SOURCES})

target_include_directories(ime_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ime_core PUBLIC
    Threads::Threads
)

set_target_properties(ime_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()