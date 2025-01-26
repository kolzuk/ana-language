#include "Parser/Parser.h"
#include "Bytecode/BytecodeGenerator.h"
#include "VirtualMachine/VirtualMachine.h"
#include "Optimizer/Optimizer.h"

#include <iostream>
#include <fstream>

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