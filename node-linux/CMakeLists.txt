cmake_minimum_required(VERSION 3.10)
project(zanni-node-linux VERSION 0.1.0 LANGUAGES C)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core ${CMAKE_CURRENT_BINARY_DIR}/core_build)

add_executable(zanni-node-linux src/main.c)

target_link_libraries(zanni-node-linux PRIVATE zanni-node-core)
