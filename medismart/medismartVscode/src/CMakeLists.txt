# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/../include/local_config.h")
    message(FATAL_ERROR "Missing local_config.h. From medismart, run: node scripts/configure.cjs --firmware")
endif()

idf_component_register(SRCS ${app_sources} INCLUDE_DIRS "../include")
