cmake_minimum_required(VERSION 3.5)
project(cplusplus)

set(DEFAULT_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-w")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g -ggdb ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-g ${CMAKE_CXX_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# glog
find_package(glog REQUIRED)
include_directories(${Glog_INCLUDE_DIRS})

# yaml-cpp
find_package(yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIR})

set(third_party_libs
    glog gflags
    ${YAML_CPP_LIBRARIES}
)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/LoopFunc
    ${PROJECT_SOURCE_DIR}/ThreadPool
)

# add_library(LoopFunc STATIC LoopFunc/LoopFunc.cc LoopFunc/LoopFunc.h)
add_library(LoopFunc SHARED LoopFunc/LoopFunc.cc LoopFunc/LoopFunc.h)
target_link_libraries(LoopFunc pthread)

add_executable(TestThdPool test/TestThdPool.cc)
target_link_libraries(TestThdPool pthread)

add_executable(testcpu test/testcpu.cc)
target_link_libraries(testcpu pthread)

add_executable(test_loopfunc test/test_loopfunc.cc)
target_link_libraries(test_loopfunc LoopFunc)
