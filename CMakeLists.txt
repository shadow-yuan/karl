cmake_minimum_required(VERSION 3.8)

project(karl)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (WIN32)
  add_definitions(-D_UNICODE -DUNICODE)
endif (WIN32)

add_subdirectory(src)
add_subdirectory(example)
