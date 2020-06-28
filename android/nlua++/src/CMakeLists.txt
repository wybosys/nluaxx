cmake_minimum_required(VERSION 3.1)
project(tester)

# 获得项目根目录
get_filename_component(LIB_DIR ../../../src/ ABSOLUTE)

# 添加lua解释器目录
add_subdirectory(${LIB_DIR}/lua lua)

# 构造测试so
add_library(tester SHARED test/test.cpp)
target_link_libraries(tester lua)
