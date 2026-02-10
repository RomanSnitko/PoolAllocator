cmake_minimum_required(VERSION 3.16)

add_library(task2_lib
    src/Pool.cpp
    src/PoolAllocator.cpp
)

target_include_directories(task2_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(task2_app
    src/main.cpp
)

target_link_libraries(task2_app PRIVATE task2_lib)
