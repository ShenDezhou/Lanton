cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/include/)
aux_source_directory(. DIR_SRCS)
add_executable(test ${DIR_SRCS})

target_link_libraries(test Lanton)

