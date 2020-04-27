cmake_minimum_required(VERSION 2.8)
project(nlua++)

set(CMAKE_CXX_STANDARD 13)

add_library(nlua++
        core.cpp
        fs.cpp
        variant.cpp
        value.cpp
        nlua++.cpp
        )
target_link_libraries(nlua++ lua5.1)

add_executable(test_nlua++
        test.cpp)
target_link_libraries(test_nlua++ nlua++ UnitTest++)

