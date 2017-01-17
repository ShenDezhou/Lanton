cmake_minimum_required(VERSION 2.8)
project(Lanton)

set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -std=c99")
set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} -std=c++99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_FLAGS}")

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/test)
