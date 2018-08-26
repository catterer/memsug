cmake_minimum_required(VERSION 3.11)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall")
project(memsug)
 
add_definitions(-DROOT="${CMAKE_HOME_DIRECTORY}")

include_directories(include ~/googletest/.install/include)
link_directories(~/googletest/.install/lib64)
 
set(PREFTREE
        src/preftree.cc
        src/text.cc
        src/memsug.cc
        src/utf.cc)

add_library(memsug ${PREFTREE})

set(TESTS src/tests.cc)
add_executable(tests ${TESTS})
target_link_libraries(tests memsug)
Target_link_libraries(tests gtest)
Target_link_libraries(tests pthread)
