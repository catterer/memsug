cmake_minimum_required(VERSION 3.11)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall")
project(memsug)
 
add_definitions(-DROOT="${CMAKE_HOME_DIRECTORY}")

# office
include_directories(include ~/googletest/.install/include)
link_directories(~/googletest/.install/lib64)
include_directories(include ~/boost_install/include)
link_directories(~/boost_install/lib)
 
set(PREFTREE
        src/preftree.cc
        src/text.cc
        src/memsug.cc
        src/utf.cc)

add_library(memsug ${PREFTREE})

add_executable(tests src/tests.cc)
target_link_libraries(tests memsug)
Target_link_libraries(tests gtest)
Target_link_libraries(tests pthread)

add_executable(memsugtool src/main.cc)
target_link_libraries(memsugtool memsug)
Target_link_libraries(memsugtool gtest)
Target_link_libraries(memsugtool pthread)
Target_link_libraries(memsugtool boost_program_options)
