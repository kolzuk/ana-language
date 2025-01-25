#include "VirtualMachine/VirtualMachine.h"

int main() {
  VirtualMachine vm(100000);

  std::vector<std::pair<Operation, std::vector<std::string>>> byteCode = {
      {FUN_BEGIN, {"generateArray", "integer", "n"}}, // 1
      {LOAD, {"n"}}, // 2
      {NEW_ARRAY, {"a"}}, // 3
      {PUSH, {"0"}}, // 25
      {STORE, {"i"}}, // 26
      {LABEL, {"condition"}},
      {LOAD, {"n"}}, // 27
      {LOAD, {"i"}}, // 28
      {CMP, {}}, // 40
      {JUMP_GE, {"after"}}, // 30
      {LOAD, {"i"}}, // 31
      {LOAD, {"n"}}, // 32
      {SUB, {}}, // 33
      {LOAD, {"i"}}, // 34
      {STORE_IN_INDEX, {"a"}}, // 35
      {LOAD, {"i"}}, // 36
      {PUSH, {"1"}}, // 37
      {ADD, {}}, // 38
      {STORE, {"i"}}, // 39
      {JUMP, {"condition"}}, // 40
      {LABEL, {"after"}},
      {ARRAY_LOAD, {"a"}}, // 41
      {RETURN, {}}, // 42
      {FUN_END, {}}, // 43

      {FUN_BEGIN, {"printArray", "array", "arr", "integer", "n"}}, // 44
      {PUSH, {"0"}}, // 45
      {STORE, {"i"}}, // 46
      {LABEL, {"condition"}},
      {LOAD, {"n"}}, // 47
      {LOAD, {"i"}}, // 48
      {CMP, {}}, // 49
      {JUMP_GE, {"after"}}, // 50
      {LOAD, {"i"}}, // 51
      {LOAD_FROM_INDEX, {"arr"}}, // 52
      {PRINT, {}}, // 53
      {LOAD, {"i"}}, // 54
      {PUSH, {"1"}}, // 55
      {ADD, {}}, // 56
      {STORE, {"i"}}, // 57
      {JUMP, {"condition"}}, // 58
      {LABEL, {"after"}},
      {ARRAY_LOAD, {"arr"}}, // 59
      {RETURN, {}}, // 60
      {FUN_END, {}}, // 61

      {FUN_BEGIN, {"main"}}, // 62
      {PUSH, {"10000"}}, // 63
      {PUSH, {"10000"}}, // 64
      {CALL, {"generateArray"}}, // 64
      {CALL, {"printArray"}}, // 65
      {RETURN, {}}, // 65
      {FUN_END, {}},
  };

  vm.Execute(byteCode);

  return (int)vm.getReturnCode();
}
