#include "Parser/Parser.h"
#include "Sema/Sema.h"
#include "Bytecode/BytecodeGenerator.h"
#include "VirtualMachine/VirtualMachine.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstring>

std::string ConvertOperationToString(Operation operation) {
  switch (operation) {
    case ADD: return "ADD";
    case SUB: return "SUB";
    case MUL: return "MUL";
    case DIV: return "DIV";
    case MOD: return "MOD";
    case PUSH: return "PUSH";
    case LOAD: return "LOAD";
    case ARRAY_LOAD: return "ARRAY_LOAD";
    case LOAD_FROM_INDEX: return "LOAD_FROM_INDEX";
    case STORE: return "STORE";
    case ARRAY_STORE: return "ARRAY_STORE";
    case STORE_IN_INDEX: return "STORE_IN_INDEX";
    case NEW_ARRAY: return "NEW_ARRAY";
    case PRINT: return "PRINT";
    case CALL: return "CALL";
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

void compileFile(const std::string& SourceFile) {
  std::cout << "Compiling... " << SourceFile << '\n';

  std::ifstream File(SourceFile);
  if (!File) {
    std::cerr << "Error opening file" << std::endl;
    return;
  }

  std::string Buffer((std::istreambuf_iterator<char>(File)), std::istreambuf_iterator<char>());

  Lexer Lexer(Buffer);
  Parser Parser(Lexer);
  AST* Tree = Parser.parse();
  if (!Tree || Parser.hasError()) {
    std::cerr << "Syntax errors occured\n";
    return;
  }
//  Sema Sema;
//  if (Sema.semantic(Tree)) {
//    std::cerr << "Semantic errors occured\n";
//    return;
//  }

  BytecodeGenerator CodeGen;
  auto Bytecode = CodeGen.generate(*Tree);
  for (int i = 0; i < Bytecode.size(); ++i) {
    std::cout << ConvertOperationToString(Bytecode[i].first) << ' ';
    for (int j = 0; j < Bytecode[i].second.size(); ++j) {
      std::cout << Bytecode[i].second[j] << ' ';
    }

    std::cout << '\n';
  }
  VirtualMachine vm(100000);
  vm.Execute(Bytecode);
  File.close();
}

namespace fs = std::filesystem;

void compileAllFilesInDir(const std::string& Dir) {
  if (!fs::exists(Dir) || !fs::is_directory(Dir)) {
    std::cerr << "Not found folder " << Dir << "\n";
    return;
  }

  for (const auto& File : fs::directory_iterator(Dir)) {
    if (fs::is_regular_file(File) && File.path().extension() == ".ana") {
      compileFile(File.path().string());
    }
  }
}

int main(int argc, const char** argv) {
  if (argc < 2) {
    std::cerr << "Input file as argument expected\n";
    return -1;
  }

  if (strcmp(argv[1], "-all") == 0) {
    compileAllFilesInDir("examples");
  } else {
    for (int i = 1; i < argc; i++) {
      compileFile(argv[i]);
    }
  }
  return 0;
}