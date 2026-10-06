cmake_minimum_required(VERSION 3.20)
project("lab2" CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_executable("${PROJECT_NAME}" "lab2.cpp")
target_link_libraries("${PROJECT_NAME}" PRIVATE OpenMP::OpenMP_CXX)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options("${PROJECT_NAME}" PRIVATE /W4 /utf-8)
else()
  target_compile_options("${PROJECT_NAME}" PRIVATE -Wall -Wextra)
endif()