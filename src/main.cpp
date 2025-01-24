#include "Jit/JitInterpreter.h"

int main() {
  std::vector<std::pair<Operation, std::vector<std::string>>> bytecode = {
      {Operation::FUN_BEGIN, {"factorial", "x"}},
      {Operation::PUSH, {"1"}},
      {Operation::LOAD, {"x"}},
      {Operation::CMP, {}},
      {Operation::JUMP_GT, {"after"}},
      {Operation::PUSH, {"1"}},
      {Operation::RETURN, {}},
      {Operation::LABEL, {"after"}},
      {Operation::LOAD, {"x"}},
      {Operation::PRINT, {}},
      {Operation::PUSH, {"1"}},
      {Operation::LOAD, {"x"}},
      {Operation::SUB, {}},
      {Operation::CALL, {"factorial"}},
      {Operation::LOAD, {"x"}},
      {Operation::MUL, {}},
      {Operation::RETURN, {}},
      {Operation::FUN_END, {}},

      {Operation::FUN_BEGIN, {"main"}},
      {Operation::PUSH, {"20"}},
      {Operation::CALL, {"factorial"}},
      {Operation::PRINT, {}},
      {Operation::RETURN, {}},
      {Operation::FUN_END, {}},
      };

  JitInterpreter Interpreter;
  Interpreter.execute(bytecode);
}
