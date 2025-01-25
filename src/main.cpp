#include "VirtualMachine/VirtualMachine.h"

int main() {
  VirtualMachine vm(10000);

  std::vector<std::pair<Operation, std::vector<std::string>>> byteCode = {
      {FUN_BEGIN, {"factorial", "integer", "n"}},
      {LOAD, {"n"}},
      {PUSH, {"0"}},
      {CMP, {}},
      {JUMP_NE, {"6"}},
      {PUSH, {"1"}},
      {RETURN, {}},
      {LOAD, {"n"}},
      {PUSH, {"1"}},
      {LOAD, {"n"}},
      {SUB, {""}},
      {CALL, {"factorial"}},
      {MUL, {}},
      {RETURN, {}},
      {FUN_END, {}},
      {FUN_BEGIN, {"main"}},
      {PUSH, {"20"}},
      {CALL, {"factorial"}},
      {PRINT, {}},
      {PUSH, {"0"}},
      {RETURN, {}},
      {FUN_END, {}},
  };

  vm.Execute(byteCode);

  return (int)vm.getReturnCode();
}
