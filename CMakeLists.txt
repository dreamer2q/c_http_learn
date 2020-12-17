cmake_minimum_required(VERSION 3.0.0)

project(http VERSION 0.1.0)

aux_source_directory(. SRCS)

add_subdirectory(vendor/sds)

add_executable(http ${SRCS})

target_link_libraries(http wsock32 ws2_32)
target_link_libraries(http sds)
