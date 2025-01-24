#include "Jit/JitInterpreter.h"

int64_t asInt64(const std::string& Value) {
  return std::stoll(Value);
}

void JitInterpreter::execute(const std::vector<std::pair<Operation, std::vector<std::string>>>& Bytecode) {
  for (auto& Command : Bytecode) {
    switch (Command.first) {
      case ADD: Jit.add();
        break;
      case SUB: Jit.sub();
        break;
      case MUL: Jit.mul();
        break;
      case DIV: Jit.div();
        break;
      case MOD: Jit.mod();
        break;
      case PUSH: Jit.push(asInt64(Command.second[0]));
        break;
      case ASSIGN: Jit.assign(Command.second[0]);
        break;
      case ASSIGN_BY_INDEX: Jit.assignByIndex(Command.second[0]);
        break;
      case LOAD: Jit.load(Command.second[0]);
        break;
      case LOAD_BY_INDEX: Jit.loadByIndex(Command.second[0]);
        break;
      case NEW_ARRAY: Jit.allocNewArray(Command.second[0]);
        break;
      case PRINT: Jit.print();
        break;
      case CALL: Jit.addFunctionCall(Command.second[0]);
        break;
      case RETURN: Jit.callGC();
        Jit.addReturn();
        break;
      case GOTO: Jit.addGoto(Command.second[0]);
        break;
      case CMP: Jit.cmp();
        break;
      case JUMP_EQ: Jit.jumpEq(Command.second[0]);
        break;
      case JUMP_NE: Jit.jumpNe(Command.second[0]);
        break;
      case JUMP_LE: Jit.jumpLe(Command.second[0]);
        break;
      case JUMP_LT: Jit.jumpLt(Command.second[0]);
        break;
      case JUMP_GE: Jit.jumpGe(Command.second[0]);
        break;
      case JUMP_GT: Jit.jumpGt(Command.second[0]);
        break;
      case LABEL: Jit.label(Command.second[0]);
        break;
      case FUN_BEGIN: Jit.addFunction(Command.second);
        if (Command.second[0] == "main") {
          Jit.initGC();
        }
        break;
      case FUN_END: Jit.endFunction();
        break;
    }
  }

  auto MainFunc = Jit.lookupMain();
  MainFunc();
}
