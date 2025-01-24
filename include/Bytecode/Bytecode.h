#ifndef BYTECODE_H
#define BYTECODE_H

enum Operation : char {
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,

  PUSH,
  STORE,
  ARRAY_STORE,
  LOAD,
  ARRAY_LOAD,

  NEW_ARRAY,
  PRINT,
  CALL,
  RETURN,

  GOTO,
  CMP,
  JUMP_EQ,
  JUMP_NE,
  JUMP_LT,
  JUMP_LE,
  JUMP_GT,
  JUMP_GE,
  LABEL,

  FUN_BEGIN,
  FUN_END,
};

#endif //BYTECODE_H
