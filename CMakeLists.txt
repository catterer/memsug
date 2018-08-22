cmake_minimum_required(VERSION 3.11)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall")
project(preftree)
 
include_directories(include ~/googletest/.install/include)
link_directories(~/googletest/.install/lib64)
 
set(PREFTREE src/preftree.cc)
add_library(preftree ${PREFTREE})

set(PREFTREE_TEST src/preftree.test.cc)
add_executable(preftree_test ${PREFTREE_TEST})
target_link_libraries(preftree_test preftree)
Target_link_libraries(preftree_test gtest)
Target_link_libraries(preftree_test pthread)
