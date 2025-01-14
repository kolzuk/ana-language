#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;
using namespace llvm::orc;

static cl::opt<std::string>
    InputFile(cl::Positional, cl::Required,
              cl::desc("<input-file>"));

std::unique_ptr<Module> loadModule(StringRef Filename, LLVMContext& Ctx, const char* ProgName) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Mod = parseIRFile(Filename, Err, Ctx);
  if (!Mod) {
    Err.print(ProgName, errs());
    exit(-1);
  }
  return Mod;
}

ExitOnError ExitOnErr;

int main(int argc, char* argv[]) {
  InitLLVM X(argc, argv);

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  cl::ParseCommandLineOptions(argc, argv, "jit\n");
  auto JIT = ExitOnErr(LLJITBuilder().create());
  auto Ctx = std::make_unique<LLVMContext>();\
  std::unique_ptr<Module> M = loadModule(InputFile, *Ctx, argv[0]);

  auto RT = JIT->getMainJITDylib().createResourceTracker();
  auto TSM = ThreadSafeModule(std::move(M), std::move(Ctx));
  ExitOnErr(JIT->addIRModule(RT, std::move(TSM)));

  auto CalcExprCall = ExitOnErr(JIT->lookup("main"));
  int (* UserFnCall)() = CalcExprCall.toPtr<int(*)()> ();
  outs() << "main returned " << UserFnCall() << "\n";
  ExitOnErr(RT->remove());
  return 0;
}


