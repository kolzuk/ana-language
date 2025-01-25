#include "Parser/Parser.h"
#include "Sema/Sema.h"
#include "Bytecode/BytecodeGenerator.h"
#include "Jit/JitInterpreter.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstring>

void compileFile(const std::string& SourceFile) {
  printf("Compiling... %s\n", SourceFile.c_str());
//  std::cout << "Compiling... " << SourceFile.c_str() << "\n";

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
  Sema Sema;
  if (Sema.semantic(Tree)) {
    std::cerr << "Semantic errors occured\n";
    return;
  }

  BytecodeGenerator CodeGen;
  auto Bytecode = CodeGen.generate(*Tree);
//  std::cout << "Execution...\n";
  JitInterpreter Interpreter;
  Interpreter.execute(Bytecode);
//  std::cout << "Success\n";
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

//int main() {
//  std::vector<std::pair<Operation, std::vector<std::string>>> bytecode = {
//      {Operation::FUN_BEGIN, {"factorial", "x"}},
//      {Operation::PUSH, {"1"}},
//      {Operation::LOAD, {"x"}},
//      {Operation::CMP, {}},
//      {Operation::JUMP_GT, {"after"}},
//      {Operation::PUSH, {"1"}},
//      {Operation::RETURN, {}},
//      {Operation::LABEL, {"after"}},
//      {Operation::LOAD, {"x"}},
//      {Operation::PRINT, {}},
//      {Operation::PUSH, {"1"}},
//      {Operation::LOAD, {"x"}},
//      {Operation::SUB, {}},
//      {Operation::CALL, {"factorial"}},
//      {Operation::LOAD, {"x"}},
//      {Operation::MUL, {}},
//      {Operation::RETURN, {}},
//      {Operation::FUN_END, {}},
//
//      {Operation::FUN_BEGIN, {"main"}},
//      {Operation::PUSH, {"20"}},
//      {Operation::CALL, {"factorial"}},
//      {Operation::PRINT, {}},
//      {Operation::RETURN, {}},
//      {Operation::FUN_END, {}},
//      };
//
//  JitInterpreter Interpreter;
//  Interpreter.execute(bytecode);
//}
