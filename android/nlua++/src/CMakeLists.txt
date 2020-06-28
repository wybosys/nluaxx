cmake_minimum_required(VERSION 3.1)
project(tester)

# 获得项目根目录
get_filename_component(LIB_DIR ../../../src/ ABSOLUTE)

# 添加lua解释器目录
add_subdirectory(${LIB_DIR}/lua lua)

# 添加ajni++项目
add_subdirectory(${LIB_DIR}/ajni++ ajni++)

# 添加nlua项目
include_directories(${LIB_DIR}/lua/5.3)
add_subdirectory(${LIB_DIR} nlua++)

# 构造测试so
add_library(tester SHARED test.cpp ${LIB_DIR}/android_link.cpp)
target_link_libraries(tester lua nlua++ ajni++)
