cmake_minimum_required(VERSION 3.0)

project(LI_STL)
add_compile_options(--std=c++11)
# 添加头文件路径
include_directories(${CMAKE_SOURCE_DIR}/include)

# 可执行文件生成
add_executable(test_vector
    src/test_vector.cpp
)

add_executable(test_deque
    src/test_deque.cpp
)

add_executable(test_rb_tree
    src/test_rb_tree.cpp
)

add_executable(test_map
    src/test_map.cpp
)
