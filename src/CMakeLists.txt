add_llvm_executable(ana_language main.cpp
        Parser/Parser.cpp
        Lexer/Lexer.cpp
        Sema/Sema.cpp
        Codegen/CodeGen.cpp
        Codegen/Optimizer.cpp
)

target_include_directories(ana_language PRIVATE "${PROJECT_SOURCE_DIR}/include")

