cmake_minimum_required(VERSION 3.28.1)
project("jit")

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        asmjit
        GIT_REPOSITORY https://github.com/asmjit/asmjit.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(asmjit)

add_executable(jit main.cpp
        Interpreter.cpp)
target_link_libraries(jit PUBLIC asmjit)
