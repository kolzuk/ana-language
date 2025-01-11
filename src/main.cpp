#include "Codegen/Codegen.h"
#include "Parser/Parser.h"
#include "Sema/Sema.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/Support/SourceMgr.h>

static llvm::cl::opt<std::string>
    InputFile(llvm::cl::Positional,
              llvm::cl::desc("<input filename>"),
              llvm::cl::init(""));

int main(int argc, const char** argv) {
  llvm::InitLLVM X(argc, argv);
  llvm::cl::ParseCommandLineOptions(
      argc, argv, "ana language compiler\n");

  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
      FileOrErr = llvm::MemoryBuffer::getFile(InputFile);
  if (std::error_code BufferError = FileOrErr.getError()) {
    llvm::errs() << "Error reading " << InputFile << ": "
                 << BufferError.message() << "\n";
    return 1;
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
    return 1;
  }
  Sema Sema;
  if (Sema.semantic(Tree)) {
    llvm::errs() << "Semantic errors occured\n";
    return 1;
  }

  CodeGen::compile(Tree, InputFile);
  return 0;
}