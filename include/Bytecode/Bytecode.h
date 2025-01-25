#ifndef BYTECODE_H
#define BYTECODE_H

enum Operation : char {
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,

  PUSH,
  LOAD,
  LOAD_FROM_INDEX, // LOAD_FROM_INDEX "a" 1 <-> a[1]
  STORE,           // STORE "a" 2           <-> a = 2;
  STORE_IN_INDEX,  // ARRAY_STORE "a"       <-> a[3] = stack.top()
  NEW_ARRAY,       // NEW_ARRAY "ab"   <-> a = new array[3]
  PRINT,           // PRINT                 <-> print(a)
  CALL,            // CALL
  RETURN,

  JUMP,            // JUMP <label>
  CMP,             // CMP
  JUMP_EQ,         // JUMP <label>
  JUMP_NE,         //
  JUMP_LT,
  JUMP_LE,
  JUMP_GT,
  JUMP_GE,

  FUN_BEGIN,
  FUN_END,
};

#endif //BYTECODE_H
