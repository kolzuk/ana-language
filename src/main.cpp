#include "Parser/Parser.h"
#include "Sema/Sema.h"
#include "Bytecode/BytecodeGenerator.h"
#include "Jit/JitInterpreter.h"

#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"

#include <filesystem>

void compileFile(const std::string& SourceFile) {
  llvm::outs() << "Compiling... " << SourceFile << "\n";

  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
      FileOrErr = llvm::MemoryBuffer::getFile(SourceFile);
  if (std::error_code BufferError = FileOrErr.getError()) {
    llvm::errs() << "Error reading " << SourceFile << ": "
                 << BufferError.message() << "\n";
    return;
  }

  llvm::SourceMgr SrcMgr;
  SrcMgr.AddNewSourceBuffer(std::move(*FileOrErr),
                            llvm::SMLoc());
  auto Buffer = SrcMgr.getMemoryBuffer(SrcMgr.getMainFileID())->getBuffer();

  Lexer Lexer(Buffer);
  Parser Parser(Lexer);
  AST* Tree = Parser.parse();
  if (!Tree || Parser.hasError()) {
    llvm::errs() << "Syntax errors occured\n";
    return;
  }
  Sema Sema;
  if (Sema.semantic(Tree)) {
    llvm::errs() << "Semantic errors occured\n";
    return;
  }

  BytecodeGenerator CodeGen;
  auto Bytecode = CodeGen.generate(*Tree);
  JitInterpreter Interpreter;
  Interpreter.execute(Bytecode);
  llvm::outs() << "Success\n";
}

namespace fs = std::filesystem;

void compileAllFilesInDir(const std::string& Dir) {
  if (!fs::exists(Dir) || !fs::is_directory(Dir)) {
    llvm::errs() << "Not found folder " << Dir << "\n";
    return;
  }

  for (const auto& File : fs::directory_iterator(Dir)) {
    if (fs::is_regular_file(File) && File.path().extension() == ".ana") {
      compileFile(File.path().string());
    }
  }
}

int main(int argc, const char** argv) {
  llvm::InitLLVM X(argc, argv);

  if (argc < 2) {
    llvm::errs() << "Input file as argument expected\n";
    return -1;
  }

  if (std::strcmp(argv[1], "-all") == 0) {
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
