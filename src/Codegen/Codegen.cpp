#include "Codegen/Codegen.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"

namespace {
class ToIRVisitor : public ASTVisitor {
  Module* M;
  IRBuilder<> Builder;
  Type* VoidTy;
  Type* Int32Ty;
  PointerType* PtrTy;
  Constant* Int32Zero;
  Value* V;
  StringMap<Value*> NameMap;
 public:
  ToIRVisitor(Module* M) : M(M), Builder(M->getContext()) {
    VoidTy = Type::getVoidTy(M->getContext());
    Int32Ty = Type::getInt32Ty(M->getContext());
    PtrTy = PointerType::getUnqual(M->getContext());
    Int32Zero = ConstantInt::get(Int32Ty, 0, true);
  }

  void run(AST* Tree) {
    FunctionType* MainFty = FunctionType::get(
        Int32Ty, {Int32Ty, PtrTy}, false);
    Function* MainFn = Function::Create(
        MainFty, GlobalValue::ExternalLinkage,
        "main", M);

    BasicBlock* BB = BasicBlock::Create(M->getContext(),
                                        "entry", MainFn);
    Builder.SetInsertPoint(BB);

    Tree->accept(*this);

    Builder.CreateRet(V);
  }

  virtual void visit(StatementsSequence& Node) override {
    auto Statements = Node.getStatements();

    for (auto S : Statements)
      S->accept(*this);
  }

  virtual void visit(VariableDeclaration& Node) override {
    Node.getExpr()->accept(*this);

    NameMap[Node.getIdentifier()] = V;
  }

  // TODO
  virtual void visit(FunctionDeclaration& Node) override {
  }

  virtual void visit(Factor& Node) override {
    if (Node.getKind() == Factor::Ident) {
      V = NameMap[Node.getVal()];
    } else {
      int intval;
      Node.getVal().getAsInteger(10, intval);
      V = ConstantInt::get(Int32Ty, intval, true);
    }
  }

  virtual void visit(BinaryOp& Node) override {
    Node.getLeft()->accept(*this);
    Value* Left = V;
    Node.getRight()->accept(*this);
    Value* Right = V;
    switch (Node.getOperator()) {
      case BinaryOp::Plus:V = Builder.CreateNSWAdd(Left, Right);
        break;
      case BinaryOp::Minus:V = Builder.CreateNSWSub(Left, Right);
        break;
      case BinaryOp::Mul:V = Builder.CreateNSWMul(Left, Right);
        break;
      case BinaryOp::Div:V = Builder.CreateSDiv(Left, Right);
        break;
    }
  };
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