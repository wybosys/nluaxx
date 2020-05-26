cmake_minimum_required(VERSION 2.8)
project(nlua++)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)
set(NLUA_SRCS
        src/core.cpp
        src/variant.cpp
        src/value.cpp
        src/str.cpp
        src/fs.cpp
        src/nlua++.cpp
        )

macro(build_nlua ver lib DEFS)
    add_library(nlua++-${ver} STATIC ${NLUA_SRCS})
    target_compile_definitions(nlua++-${ver} PUBLIC ${DEFS})
    target_link_libraries(nlua++-${ver} ${lib})
    add_executable(test_nlua++-${ver}
            test/timer.cpp
            test/test.cpp)
    target_link_libraries(test_nlua++-${ver} nlua++-${ver} UnitTest++ pthread)
endmacro()

build_nlua(jit luajit-5.1 NLUA_JIT)
build_nlua(5.1 lua5.1 NLUA_51)
build_nlua(5.2 lua5.2 NLUA_52)
build_nlua(5.3 lua5.3 NLUA_53)
