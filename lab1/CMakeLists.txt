cmake_minimum_required(VERSION 4.1)

project("lab1" CXX)

add_executable("${PROJECT_NAME}")

target_compile_features("${PROJECT_NAME}" PUBLIC cxx_std_23)

target_sources("${PROJECT_NAME}" PRIVATE "lab1.cpp")