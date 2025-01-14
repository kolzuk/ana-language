#include "Codegen/CodeGen.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
class ToIRVisitor : public ASTVisitor {
  Module* M;
  IRBuilder<> Builder;
  Type* VoidTy;
  Type* Int64Ty;
  ArrayType* ArrayTy;
  PointerType* PtrTy;
  Value* V = nullptr;
  Function* CurrentFunction = nullptr;
  StringMap<Function*> FunctionsMap;
  StringMap<Value*> NameMap;
  Function* PrintFunction;
 public:
  ToIRVisitor(Module* M) : M(M), Builder(M->getContext()) {
    VoidTy = Type::getVoidTy(M->getContext());
    Int64Ty = Type::getInt64Ty(M->getContext());
    PtrTy = PointerType::get(Int64Ty, 0);
  }

  void run(AST* Tree) {
    PrintFunction = Function::Create(
        FunctionType::get(
            Type::getInt32Ty(M->getContext()),  // Возвращаемое значение printf
            PointerType::get(Type::getInt8Ty(M->getContext()), 0), // char*
            true), // printf имеет переменное количество аргументов
        Function::ExternalLinkage,
        "printf",
        M);

    Tree->accept(*this);
  }

  void visit(CompilationUnitAST& Node) override {
    for (auto Declaration : Node.Declarations) {
      Declaration->accept(*this);
    }
  }

  void visit(VariableDeclarationAST& Node) override {
    TypeAST::TypeKind Type = Node.T->Type;
    IdentifierAST* Name = Node.Ident;

    AllocaInst* Alloca = nullptr;
    if (Type == TypeAST::TypeKind::Array) {
      Node.T->accept(*this);

      Alloca = Builder.CreateAlloca(ArrayTy, V, Name->Value);
      if (Node.Expr) {
        Node.Expr->accept(*this);
        Builder.CreateStore(V, Alloca);
      }
    } else if (Type == TypeAST::TypeKind::Integer) {
      Alloca = Builder.CreateAlloca(Int64Ty, nullptr, Name->Value);
      Alloca->setAlignment(Align(8));
      if (Node.Expr) {
        Node.Expr->accept(*this);
        Builder.CreateStore(V, Alloca)->setAlignment(Align(8));
      }
    }

    NameMap[Name->Value] = Alloca;
  }

  virtual void visit(FunctionDeclarationAST& Node) override {
    SmallVector<Type*> ArgTypes;
    for (auto* ArgType : Node.Arguments->Types) {
      switch (ArgType->Type) {
        case TypeAST::Array:
          ArgTypes.push_back(PtrTy);
          break;
        case TypeAST::Integer:
          ArgTypes.push_back(Int64Ty);
          break;
        case TypeAST::Void:
          ArgTypes.push_back(VoidTy);
          break;
      }
    }

    FunctionType* FunctionTy = nullptr;

    switch (Node.ReturnType->Type) {
      case TypeAST::Array:
        FunctionTy = FunctionType::get(PtrTy, ArgTypes, false);
        break;
      case TypeAST::Integer:
        FunctionTy = FunctionType::get(Int64Ty, ArgTypes, false);
        break;
      case TypeAST::Void:
        FunctionTy = FunctionType::get(VoidTy, ArgTypes, false);
    }

    llvm::StringRef FunctionName = Node.Ident->Value;
    CurrentFunction = Function::Create(
        FunctionTy, GlobalValue::ExternalLinkage, FunctionName, M);
    NameMap[FunctionName] = CurrentFunction;
    BasicBlock* BB = BasicBlock::Create(M->getContext(),"entry", CurrentFunction);
    Builder.SetInsertPoint(BB);

    unsigned Idx = 0;
    for (auto& Arg : CurrentFunction->args()) {
      llvm::StringRef ArgName = Node.Arguments->Idents[Idx]->Value;
      Arg.setName(ArgName);
      ++Idx;
      auto* Alloca = Builder.CreateAlloca(Int64Ty, nullptr);
      Builder.CreateStore(&Arg, Alloca)->setAlignment(Align(8));
      NameMap[ArgName] = Alloca;
    }

    Node.Body->accept(*this);
  }

  virtual void visit(StatementSequenceAST& Node) override {
    for (auto Declaration : Node.Statements) {
      Declaration->accept(*this);
    }
  }

  virtual void visit(IfStatementAST& Node) override {;
    Node.Condition->accept(*this);
    Value* CondResult = V;

    llvm::BasicBlock* IfBodyBB = llvm::BasicBlock::Create(
        M->getContext(), "if.body", CurrentFunction);
    llvm::BasicBlock* AfterIfBB = llvm::BasicBlock::Create(
        M->getContext(), "after.if", CurrentFunction);

    Builder.CreateCondBr(CondResult, IfBodyBB, AfterIfBB);

    Builder.SetInsertPoint(IfBodyBB);
    Node.Body->accept(*this);

    Builder.SetInsertPoint(AfterIfBB);
  }
  virtual void visit(WhileStatementAST&) override {
  }

  virtual void visit(ReturnStatementAST& Node) override {
    Node.Expr->accept(*this);
    Builder.CreateRet(V);
  }

  virtual void visit(AssignStatementAST&) override {
  }

  virtual void visit(PrintStatementAST& Node) override {
    auto FormatStr = Builder.CreateGlobalStringPtr("%ld\n");

    Node.Expr->accept(*this);
    Value* ValueToPrint = V;

    Builder.CreateCall(PrintFunction, {FormatStr, ValueToPrint});
  }


  virtual void visit(IntegerTypeAST&) override {
  }

  virtual void visit(ArrayTypeAST& Node) override {
    Node.Size->accept(*this);
    Value* SizeValue = V;

    ArrayTy = ArrayType::get(Int64Ty, cast<ConstantInt>(SizeValue)->getZExtValue());
  }

  virtual void visit(ArgumentsListAST&) override {
  }
  virtual void visit(ExpressionsListAST&) override {
  }

  void visit(ExpressionAST& Node) override {
    Node.LHS->accept(*this);
    Value* LHS = V;
    if (Node.RHS) {
      Node.RHS->accept(*this);
      Value* RHS = V;
      switch (Node.Rel->RelKind) {
        case RelationAST::Equal:
          V = Builder.CreateICmpEQ(LHS, RHS);
          break;
        case RelationAST::NotEqual:
          V = Builder.CreateICmpNE(LHS, RHS);
          break;
        case RelationAST::Less:
          V = Builder.CreateICmpSLT(LHS, RHS);
          break;
        case RelationAST::LessEq:
          V = Builder.CreateICmpSLE(LHS, RHS);
          break;
        case RelationAST::Greater:
          V = Builder.CreateICmpSGT(LHS, RHS);
          break;
        case RelationAST::GreaterEq:
          V = Builder.CreateICmpSGE(LHS, RHS);
          break;
      }
    } else {
      V = LHS;
    }
  }

  void visit(RelationAST&) override {
  }

  void visit(SimpleExpressionAST& Node) override {
    Node.Trm->accept(*this);
    Value* Result = V;
    auto AddOps = Node.AddOperators;
    auto Terms = Node.Terms;
    for (size_t i = 0; i < AddOps.size(); ++i) {
      Terms[i]->accept(*this);
      Value* TermVal = V;
      switch (AddOps[i]->AddOperatorKind) {
        case AddOperatorAST::Plus:
          Result = Builder.CreateAdd(Result, TermVal);
          break;
        case AddOperatorAST::Minus:
          Result = Builder.CreateSub(Result, TermVal);
          break;
      }
    }

    V = Result;
  }

  virtual void visit(AddOperatorAST&) override {
  }

  virtual void visit(TermAST& Node) override {
    Node.MulOperand->accept(*this);
    Value* Result = V;
    auto MulOps = Node.MulOperators;
    auto Factors = Node.MulOperands;
    for (size_t i = 0; i < MulOps.size(); ++i) {
      Factors[i]->accept(*this);
      Value* FactorVal = V;
      switch (MulOps[i]->MulOperatorKind) {
        case MulOperatorAST::Multiple:
          Result = Builder.CreateMul(Result, FactorVal);
          break;
        case MulOperatorAST::Divide:
          Result = Builder.CreateSDiv(Result, FactorVal);
          break;
      }
    }

    V = Result;
  }

  void visit(MulOperatorAST&) override {
  }

  void visit(MulOperandAST& Node) override {
    Node.Factor->accept(*this);
  }

  virtual void visit(UnaryOperatorAST&) override {
  }

  virtual void visit(IdentifierAST& Node) override {
    auto* Load = Builder.CreateLoad(Int64Ty, NameMap[Node.Value]);
    Load->setAlignment(Align(8));
    V = Load;
  }

  virtual void visit(IntegerLiteralAST& Node) override {
    V = ConstantInt::get(Int64Ty, std::stoll(Node.Value.str()));
  }

  virtual void visit(ArrayInitializationAST& Node) override {
    llvm::errs() << "Visiting node: " << typeid(Node).name() << "\n";

    SmallVector<Constant*> Elements;
    for (auto& Expr : Node.Exprs) {
      Expr->accept(*this);
      if (auto* ConstVal = dyn_cast<Constant>(V)) {
        Elements.push_back(ConstVal);
      } else {
        llvm::errs() << "Non-constant value in array initialization.\n";
        return;
      }
    }

    ArrayTy = ArrayType::get(Int64Ty, Elements.size());
    V = ConstantArray::get(ArrayTy, ArrayRef(Elements));
  }

  virtual void visit(GetByIndexAST& Node) override {

  }

  virtual void visit(ExpressionFactorAST& Node) override {
    Node.Expr->accept(*this);
  }

  void visit(FunctionCallAST& Node) override {
    auto FunctionName = Node.Ident->Value;
    auto* CallableFunction = dyn_cast<Function>(NameMap[FunctionName]);
    llvm::SmallVector<Value*> Params;

    for (auto Param : Node.ExprList->Exprs) {
      Param->accept(*this);
      Params.push_back(V);
    }

    V = Builder.CreateCall(CallableFunction->getFunctionType(),
                       CallableFunction,
                       Params);
  }
};
}

void CodeGen::compile(AST* Tree, std::string& SourceFilename) {
  LLVMContext Ctx;
  auto* M = new Module(SourceFilename, Ctx);
  ToIRVisitor ToIR(M);
  ToIR.run(Tree);

  auto OutputFilename = SourceFilename + ".ll";
  std::error_code EC;
  sys::fs::OpenFlags OpenFlags = sys::fs::OF_None;
  auto Out = std::make_unique<llvm::ToolOutputFile>(
      OutputFilename, EC, OpenFlags);
  if (EC) {
    llvm::errs() << EC.message() << '\n';
    return;
  }
  M->print(Out->os(), nullptr);
  Out->keep();
}