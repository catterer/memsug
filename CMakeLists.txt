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
include_directories(include ~/gperftools_install/include)
link_directories(~/gperftools_install/lib)
include_directories(include ~/libevent_install/include)
link_directories(~/libevent_install/lib)
 
set(PREFTREE
        src/preftree.cc
        src/text.cc
        src/memsug.cc
        src/valuer.cc
        src/utf.cc)

add_library(memsug ${PREFTREE})

add_executable(tests src/tests.cc)
target_link_libraries(tests memsug)
target_link_libraries(tests gtest)
target_link_libraries(tests pthread)

add_executable(memsug_tool src/memsug_tool.cc)
target_link_libraries(memsug_tool memsug)
target_link_libraries(memsug_tool gtest)
target_link_libraries(memsug_tool pthread)
target_link_libraries(memsug_tool boost_program_options)
#target_link_libraries(memsug_tool profiler)
#target_link_libraries(memsug_tool tcmalloc)

add_executable(memsug_daemon src/memsug_daemon.cc)
target_link_libraries(memsug_daemon memsug)
target_link_libraries(memsug_daemon event)
target_link_libraries(memsug_daemon boost_program_options)
