cmake_minimum_required(VERSION 2.8)
project(nlua++)

set(CMAKE_CXX_STANDARD 11)

set(NLUA_SRCS
ast.cpp
nlua++.cpp
private.cpp
value.cpp
variant.cpp)

add_subdirectory(cross)

add_library(nlua++ STATIC ${NLUA_SRCS})
target_link_libraries(nlua++ cross)
