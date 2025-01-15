#include "Codegen/CodeGen.h"
#include "Parser/Parser.h"
#include "Sema/Sema.h"

#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/Support/SourceMgr.h>

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

  CodeGen::compile(Tree, SourceFile);
  llvm::outs() << "Success\n";
}

namespace fs = std::filesystem;

void compileAllFilesInDir(const std::string& Dir) {
  if (!fs::exists(Dir) || !fs::is_directory(Dir)) {
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