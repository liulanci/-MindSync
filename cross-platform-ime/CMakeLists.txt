cmake_minimum_required(VERSION 3.15)
project(cross-platform-ime VERSION 1.0.0 LANGUAGES C CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 项目选项
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_CORE "Build core IME engine" ON)
option(BUILD_PLUGINS "Build plugin system" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# 平台检测
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    add_definitions(-DPLATFORM_WINDOWS)
elseif(APPLE)
    set(PLATFORM_MACOS TRUE)
    add_definitions(-DPLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
    add_definitions(-DPLATFORM_LINUX)
endif()

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_SOURCE_DIR}/src/plugins/common
)

# 依赖查找
find_package(Threads REQUIRED)

# 编译器选项
if(MSVC)
    add_compile_options(/W4 /WX /EHsc)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
    if(ENABLE_SANITIZERS)
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
    if(ENABLE_COVERAGE)
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()
endif()

# 核心库
if(BUILD_CORE)
    add_subdirectory(src/core)
endif()

# 插件系统
if(BUILD_PLUGINS)
    add_subdirectory(src/plugins)
endif()

# 平台适配层
if(PLATFORM_WINDOWS)
    add_subdirectory(src/platform/windows)
elseif(PLATFORM_MACOS)
    add_subdirectory(src/platform/macos)
elseif(PLATFORM_LINUX)
    add_subdirectory(src/platform/linux)
endif()

# 测试
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装配置
install(DIRECTORY ${CMAKE_SOURCE_DIR}/configs/
    DESTINATION share/${PROJECT_NAME}/configs
    FILES_MATCHING PATTERN "*.yaml" PATTERN "*.json"
)

# 包生成
include(CPack)