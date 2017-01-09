cmake_minimum_required(VERSION 2.8)

add_definitions(-fPIC)
set(SRCLIST Lanton.c csiphash.c)
include_directories(${CMAKE_SOURCE_DIR}/include/)

# Static Library Output
add_library(Lanton ${SRCLIST})
target_include_directories(Lanton INTERFACE "${CMAKE_SOURCE_DIR}/include/")
