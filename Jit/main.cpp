#include "Interpreter.h"

int main() {

  char bytecode[] {
      OpCode::FUN, 'f', 'a', 'c', 0, BytecodeType::INT_TYPE,
      BytecodeType::INT_TYPE, 'x', 0,
      0,
      OpCode::CMP_GT,
      BytecodeType::INT_TYPE, 'x', 0,
      BytecodeType::INT_LITERAL, 0, 0, 0, 0, 0, 0, 0, 1,
      'n', 'e', 'x', 't', 0,
      OpCode::RETURN,
      BytecodeType::INT_LITERAL, 0, 0, 0, 0, 0, 0, 0, 1,

      OpCode::BLOCK, 'n', 'e', 'x', 't', 0,
      OpCode::NEW_INT, 'y', 0,
      OpCode::ASSIGN,
      BytecodeType::INT_TYPE, 'y', 0,
      BytecodeType::INT_TYPE, 'x', 0,
      OpCode::SUB,
      BytecodeType::INT_TYPE, 'y', 0,
      BytecodeType::INT_LITERAL, 0, 0, 0, 0, 0, 0, 0, 1,
      // y = fac(y)
      OpCode::CALL, 'f', 'a', 'c', 0,
      BytecodeType::INT_TYPE, 'y', 0,
      BytecodeType::INT_TYPE, 'y', 0,
      OpCode::MUL,
      BytecodeType::INT_TYPE, 'x', 0,
      BytecodeType::INT_TYPE, 'y', 0,
      OpCode::RETURN,
      BytecodeType::INT_TYPE, 'x', 0,
      OpCode::FUN_END,

      // fun main() -> void
      OpCode::FUN, 'm', 'a', 'i', 'n', 0, BytecodeType::VOID_TYPE,
      0,
      OpCode::NEW_INT, 'c', 0,

      // c = fac(20)
      OpCode::CALL, 'f', 'a', 'c', 0,
      BytecodeType::INT_LITERAL, 0, 0, 0, 0, 0, 0, 0, 20,
      BytecodeType::INT_TYPE, 'c', 0,

      // print c
      OpCode::PRINT, BytecodeType::INT_TYPE, 'c', 0,

      // array[3] arr
      OpCode::NEW_ARRAY, 'a', 'r', 'r', 0,
      0, 0, 0, 0, 0, 0, 0, 3,

      OpCode::NEW_INT, 'i', 0,
      OpCode::NEW_INT, 't', 0, //  t - указатель

      // t = arr
      OpCode::ASSIGN,
      BytecodeType::INT_TYPE, 't', 0,
      BytecodeType::INT_TYPE, 'a', 'r', 'r', 0,

      OpCode::BLOCK, 'c', 'o', 'n', 'd', 0,

      // if i >= 3 goto after
      OpCode::CMP_GE,
      BytecodeType::INT_TYPE, 'i', 0,
      BytecodeType::INT_LITERAL, 0, 0, 0, 0, 0, 0, 0, 3,
      'a', 'f', 't', 'e', 'r', 0,

      // *(t + 0) = i
      OpCode::ASSIGN,
      BytecodeType::INDEX, 't', 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      BytecodeType::INT_TYPE, 'i', 0,

      // i = i + 1
      OpCode::ADD,
      BytecodeType::INT_TYPE, 'i', 0,
      BytecodeType::INT_LITERAL, 0, 0, 0, 0, 0, 0, 0, 1,

      // t = t + 8
      OpCode::ADD,
      BytecodeType::INT_TYPE, 't', 0,
      BytecodeType::INT_LITERAL, 0, 0, 0, 0, 0, 0, 0, 8,

      // goto cond
      OpCode::GOTO, 'c', 'o', 'n', 'd', 0,

      OpCode::BLOCK, 'a', 'f', 't', 'e', 'r', 0,

      // print arr[0]
      OpCode::PRINT,
      BytecodeType::INDEX, 'a', 'r', 'r', 0,
      0, 0, 0, 0, 0, 0, 0, 0,

      // print arr[1]
      OpCode::PRINT,
      BytecodeType::INDEX, 'a', 'r', 'r', 0,
      0, 0, 0, 0, 0, 0, 0, 1,

      // print arr[2]
      OpCode::PRINT,
      BytecodeType::INDEX, 'a', 'r', 'r', 0,
      0, 0, 0, 0, 0, 0, 0, 2,

      OpCode::RETURN_VOID,
      OpCode::FUN_END,
      OpCode::END
  };

  Interpreter Interpreter;
  Interpreter.execute(bytecode);
}
