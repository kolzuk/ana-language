#include <Bytecode/Bytecode.h>

std::string ConvertOperationToString(Operation operation) {
  switch (operation) {
    case ADD: return "ADD";
    case SUB: return "SUB";
    case MUL: return "MUL";
    case DIV: return "DIV";
    case MOD: return "MOD";
    case PUSH: return "PUSH";
    case INTEGER_LOAD: return "INTEGER_LOAD";
    case ARRAY_LOAD: return "ARRAY_LOAD";
    case LOAD_FROM_INDEX: return "LOAD_FROM_INDEX";
    case INTEGER_STORE: return "INTEGER_STORE";
    case ARRAY_STORE: return "ARRAY_STORE";
    case STORE_IN_INDEX: return "STORE_IN_INDEX";
    case NEW_ARRAY: return "NEW_ARRAY";
    case PRINT: return "PRINT";
    case FUN_CALL: return "FUN_CALL";
    case RETURN: return "RETURN";
    case LABEL: return "LABEL";
    case JUMP: return "JUMP";
    case CMP: return "CMP";
    case JUMP_EQ: return "JUMP_EQ";
    case JUMP_NE: return "JUMP_NE";
    case JUMP_LT: return "JUMP_LT";
    case JUMP_LE: return "JUMP_LE";
    case JUMP_GT: return "JUMP_GT";
    case JUMP_GE: return "JUMP_GE";
    case FUN_BEGIN: return "FUN_BEGIN";
    case FUN_END: return "FUN_END";
  }
}