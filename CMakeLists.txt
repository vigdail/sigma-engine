cmake_minimum_required(VERSION 3.15)
project(sigma-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

add_subdirectory(vendor)
add_subdirectory(sigma)
add_subdirectory(sandbox)
