cmake_minimum_required(VERSION 2.8)
project(lua)

add_subdirectory(5.3)

add_library(lua STATIC lua.c)
target_link_libraries(lua lua53)
