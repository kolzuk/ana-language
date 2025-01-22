#include "Interpreter.h"

int main() {

  char bytecode[] {
      // fun sum(integer a, integer b) -> integer
      OpCode::FUN, 's', 'u', 'm', 0, BytecodeType::INT_TYPE,
      BytecodeType::INT_TYPE, 'a', 0,
      BytecodeType::INT_TYPE, 'b', 0,
      0,

      OpCode::ADD,
      BytecodeType::INT_TYPE, 'a', 0,
      BytecodeType::INT_TYPE, 'b', 0,
      OpCode::RETURN, BytecodeType::INT_TYPE, 'a', 0,
      OpCode::FUN_END,

      // fun main() -> void
      OpCode::FUN, 'm', 'a', 'i', 'n', 0, BytecodeType::VOID_TYPE,
      0,
      OpCode::NEW_INT, 'c', 0,

      // c = sum(10, 32)
      OpCode::CALL, 's', 'u', 'm', 0,
      BytecodeType::INT_LITERAL, 0, 0, 0, 0, 0, 0, 0, 10,
      BytecodeType::INT_LITERAL, 0, 0, 0, 0, 0, 0, 0, 32,
      BytecodeType::INT_TYPE, 'c', 0,

      // print c
      OpCode::PRINT, BytecodeType::INT_TYPE, 'c', 0,
      OpCode::RETURN_VOID,
      OpCode::FUN_END,
      OpCode::END
  };

  Interpreter Interpreter;
  Interpreter.execute(bytecode);
}
