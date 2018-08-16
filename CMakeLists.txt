cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall")
project(preftree)
 
include_directories(include)
 
set(PREFTREE src/preftree.cc)
add_library(preftree ${PREFTREE})

set(PREFTREE_TEST src/preftree.test.cc)
add_executable(preftree_test ${PREFTREE_TEST})
target_link_libraries(preftree_test preftree)
