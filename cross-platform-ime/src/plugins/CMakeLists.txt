cmake_minimum_required(VERSION 3.15)

set(PLUGINS_COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/common/plugin_loader.cpp
)

add_library(ime_plugins_common ${PLUGINS_COMMON_SOURCES})

target_include_directories(ime_plugins_common PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)

target_link_libraries(ime_plugins_common PUBLIC
    ime_core
)

# Rime适配器插件
add_library(ime_plugin_rime
    rime_adapter/rime_adapter_plugin.cpp
)

target_include_directories(ime_plugin_rime PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/rime_adapter
)

target_link_libraries(ime_plugin_rime PUBLIC
    ime_core
    ime_plugins_common
    rime
)

# 云词库插件
add_library(ime_plugin_cloud_dict
    cloud_dict/cloud_dictionary_service.cpp
)

target_include_directories(ime_plugin_cloud_dict PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cloud_dict
)

target_link_libraries(ime_plugin_cloud_dict PUBLIC
    ime_core
    ime_plugins_common
)