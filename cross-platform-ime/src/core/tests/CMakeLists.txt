cmake_minimum_required(VERSION 3.15)

find_package(GTest REQUIRED)

add_executable(ime_core_tests
    core_tests.cpp
)

target_link_libraries(ime_core_tests PRIVATE
    ime_core
    GTest::GTest
    GTest::Main
)

target_include_directories(ime_core_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core/include
)

add_test(NAME ime_core_tests COMMAND ime_core_tests)