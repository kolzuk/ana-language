#ifndef ANA_LANGUAGE_INCLUDE_CODEGEN_OPTIMISATOR_H_
#define ANA_LANGUAGE_INCLUDE_CODEGEN_OPTIMISATOR_H_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Passes/PassBuilder.h>

class Optimizer {
  llvm::PassBuilder PassBuilder;
  llvm::LoopAnalysisManager LoopAM;
  llvm::FunctionAnalysisManager FuncAM;
  llvm::CGSCCAnalysisManager CgsccAM;
  llvm::ModuleAnalysisManager ModuleAM;
  llvm::ModulePassManager ModulePM;
 public:
  Optimizer();
  void optimize(llvm::Module& Module);
};

#endif //ANA_LANGUAGE_INCLUDE_CODEGEN_OPTIMISATOR_H_
