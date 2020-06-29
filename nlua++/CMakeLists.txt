cmake_minimum_required(VERSION 2.8)
project(nlua++)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

set(NLUA_SRCS
    ast.cpp
    nlua++.cpp
    private.cpp
    value.cpp
    variant.cpp
    android-prv.cpp
    )

add_subdirectory(cross)

add_library(nlua++ STATIC ${NLUA_SRCS})

target_compile_definitions(nlua++ PUBLIC LUA_COMPAT_5_2 LUA_COMPAT_5_1)
target_link_libraries(nlua++ cross)
