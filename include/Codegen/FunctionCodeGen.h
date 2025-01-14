//#ifndef FUNCTIONCODEGEN_H
//#define FUNCTIONCODEGEN_H
//
//#include <llvm/IR/ValueHandle.h>
//#include <llvm/IR/Instructions.h>
//
//#include "Parser/AST.h"
//
//struct BasicBlockDef {
//  llvm::DenseMap<DeclarationAST*, llvm::TrackingVH<llvm::Value>> Defs;
//  llvm::DenseMap<llvm::PHINode*, DeclarationAST*> IncompletePhis;
//  unsigned Sealed : 1;
//};
//
//void writeLocalVariable(llvm::BasicBlock* BB, DeclarationAST* Declaration, llvm::Value* Value) {
//}
//
//class FunctionCodeGen {
//  llvm::DenseMap<llvm::BasicBlock*, BasicBlockDef> CurrentDef;
//
//  void writeLocalVariable(llvm::BasicBlock *BB, DeclarationAST *Decl, llvm::Value *Val) {
//    CurrentDef[BB].Defs[Decl] = Val;
//  }
//
//  llvm::Value* readLocalVariable(llvm::BasicBlock *BB, DeclarationAST *Decl) {
//    auto Val = CurrentDef[BB].Defs.find(Decl);
//    if (Val != CurrentDef[BB].Defs.end())
//      return Val->second;
//    return readLocalVariableRecursive(BB, Decl);
//  }
//
//  llvm::Value* readLocalVariableRecursive(llvm::BasicBlock *BB, DeclarationAST *Decl) {
//    llvm::Value *Val = nullptr;
//    if (!CurrentDef[BB].Sealed) {
//      llvm::PHINode *Phi = addEmptyPhi(BB, Decl);
//      CurrentDef[BB].IncompletePhis[Phi] = Decl;
//      Val = Phi;
//    } else if (auto *PredBB = BB->getSinglePredecessor()) {
//      Val = readLocalVariable(PredBB, Decl);
//    } else {
//      llvm::PHINode *Phi = addEmptyPhi(BB, Decl);
//      writeLocalVariable(BB, Decl, Phi);
//      Val = addPhiOperands(BB, Decl, Phi);
//    }
//    writeLocalVariable(BB, Decl, Val);
//    return Val;
//  }
//
//  llvm::PHINode* addEmptyPhi(llvm::BasicBlock* BB, DeclarationAST* Decl) {
//    return BB->empty()
//           ? llvm::PHINode::Create(mapType(Decl), 0, "", BB)
//           : llvm::PHINode::Create(mapType(Decl), 0, "", &BB->front());
//  }
//
//  llvm::Value* addPhiOperands(llvm::BasicBlock *BB, DeclarationAST* Decl, llvm::PHINode *Phi) {
//    for (auto *PredBB : llvm::predecessors(BB))
//      Phi->addIncoming(readLocalVariable(PredBB, Decl), PredBB);
//    return optimizePhi(Phi);
//  }
//
//  llvm::Type* mapType(DeclarationAST* Decl, bool HonorReference) {
//    if (auto *FP = llvm::dyn_cast<FormalParameterDeclaration>(Decl)) {
//      if (FP->isVar() && HonorReference)
//        return llvm::PointerType::get(CGM.getLLVMCtx(), /*AddressSpace=*/0);
//      return CGM.convertType(FP->getType());
//    }
//    if (auto *V = llvm::dyn_cast<VariableDeclaration>(Decl))
//      return CGM.convertType(V->getType());
//    return CGM.convertType(llvm::cast<TypeDeclaration>(Decl));
//  }
//};
//
//#endif //FUNCTIONCODEGEN_H
