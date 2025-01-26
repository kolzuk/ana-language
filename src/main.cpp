#include "Parser/Parser.h"
#include "Sema/Sema.h"
#include "Bytecode/BytecodeGenerator.h"
#include "VirtualMachine/VirtualMachine.h"
#include "Codegen/Optimizer.h"

#include <iostream>
#include <fstream>

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

int main(int argc, const char** argv) {
  if (argc != 2) {
    std::cerr << "usage: anac file\n";
    return -1;
  }

  std::string SourceFile = argv[1];
  std::cout << "Compiling... " << SourceFile << '\n';

  std::ifstream File(SourceFile);
  if (!File) {
    std::cerr << "Error opening file" << std::endl;
    return -1;
  }

  std::string Buffer((std::istreambuf_iterator<char>(File)), std::istreambuf_iterator<char>());

  Lexer Lexer(Buffer);
  Parser Parser(Lexer);
  AST* Tree = Parser.parse();
  if (!Tree || Parser.hasError()) {
    std::cerr << "Syntax errors occured\n";
    return 0;
  }

  BytecodeGenerator CodeGen;
  auto Bytecode = CodeGen.generate(*Tree);
  for (int i = 0; i < Bytecode.size(); ++i) {
    std::cout << i << ' ' << ConvertOperationToString(Bytecode[i].first) << ' ';
    for (int j = 0; j < Bytecode[i].second.size(); ++j) {
      std::cout << Bytecode[i].second[j] << ' ';
    }

    std::cout << '\n';
  }
  auto vm = std::make_shared<VirtualMachine>(1000000, Bytecode);
  vm->InitializeGarbageCollector();
  vm->Execute();
  File.close();

  return vm->getReturnCode();
}