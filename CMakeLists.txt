cmake_minimum_required(VERSION 3.28.1)
project("ana_language")

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("src")
target_include_directories(ana_language PRIVATE "${PROJECT_SOURCE_DIR}/include")
