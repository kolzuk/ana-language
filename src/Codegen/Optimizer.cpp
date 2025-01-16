#include "Codegen/Optimizer.h"

Optimizer::Optimizer() {
  PassBuilder.registerModuleAnalyses(ModuleAM);
  PassBuilder.registerCGSCCAnalyses(CgsccAM);
  PassBuilder.registerFunctionAnalyses(FuncAM);
  PassBuilder.registerLoopAnalyses(LoopAM);
  PassBuilder.crossRegisterProxies(LoopAM, FuncAM, CgsccAM, ModuleAM);
  ModulePM = PassBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);
}

void Optimizer::optimize(llvm::Module& Module) {
  ModulePM.run(Module, ModuleAM);
}

