cmake_minimum_required(VERSION 3.28.1)
project("ana_language")

set(CMAKE_CXX_STANDARD 17)


# У меня здесь иначе не запускалось без этого. Необходимо починить.
# Необходимо указать нужный путь до LLVM нужной версии
set(LLVM_DIR "/opt/homebrew/opt/llvm@17/lib/cmake/llvm")
find_package(LLVM 17 REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
set(LLVM_REQUIRES_RTTI 1)
include(AddLLVM)

set(LLVM_LINK_COMPONENTS
        BitWriter
        Core
        ExecutionEngine
        MC
        MCJIT
        Support
        nativecodegen
        Passes
)

add_subdirectory("src")
target_include_directories(ana_language PRIVATE "${PROJECT_SOURCE_DIR}/include")
