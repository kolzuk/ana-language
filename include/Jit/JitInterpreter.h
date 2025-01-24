#ifndef JIT_INTERPRETER_H
#define JIT_INTERPRETER_H

#include "Bytecode/Bytecode.h"
#include "JitExecutor.h"

using namespace asmjit;

class JitInterpreter {
  JitExecutor Jit;

 public:
  void execute(const std::vector<std::pair<Operation, std::vector<std::string>>>& Bytecode);
};

#endif //JIT_INTERPRETER_H
