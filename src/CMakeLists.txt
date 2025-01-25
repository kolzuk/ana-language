add_llvm_executable(ana_language
        main.cpp
        Parser/Parser.cpp
        Lexer/Lexer.cpp
        Sema/Sema.cpp
        Codegen/CodeGen.cpp
        Codegen/Optimizer.cpp
        Bytecode/BytecodeGenerator.cpp
        Bytecode/BytecodeBuilder.cpp
        Jit/JitInterpreter.cpp
        Jit/JitExecutor.cpp
        VirtualMachine/VirtualMachine.cpp
)

include(FetchContent)

FetchContent_Declare(
        asmjit
        GIT_REPOSITORY https://github.com/asmjit/asmjit.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(asmjit)
target_link_libraries(ana_language PUBLIC asmjit)
