cmake_minimum_required(VERSION 2.8)
project(nlua++)

set(CMAKE_CXX_STANDARD 11)

set(NLUA_SRCS
        core.cpp
        fs.cpp
        variant.cpp
        value.cpp
        nlua++.cpp
        )
macro(build_nlua ver lib DEFS)
    add_library(nlua++_${ver} STATIC ${NLUA_SRCS})
    target_compile_definitions(nlua++_${ver} PUBLIC ${DEFS})
    target_link_libraries(nlua++_${ver} core ${lib})
    add_executable(test_nlua++_${ver}
            timer.cpp
            test.cpp)
    target_link_libraries(test_nlua++_${ver} core test nlua++_${ver} UnitTest++)
endmacro()

build_nlua(jit luajit-5.1 NLUA_JIT)
build_nlua(5.1 lua5.1 NLUA_51)
build_nlua(5.2 lua5.2 NLUA_52)
build_nlua(5.3 lua5.3 NLUA_53)
