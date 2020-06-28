cmake_minimum_required(VERSION 2.8)
project(cross)

set(CMAKE_CXX_STANDARD 11)

set(CROSS_SRCS
datetime.cpp
fs.cpp
str.cpp
sys.cpp
threads.cpp
timer.cpp
)

add_library(cross STATIC ${CROSS_SRCS})
target_link_libraries(cross pthread)
