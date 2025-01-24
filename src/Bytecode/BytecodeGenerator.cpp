#include "Bytecode/BytecodeGenerator.h"
#include "Parser/AST.h"

class ToBytecode : public ASTVisitor {

 public:
  char* generate(AST& Ast);

  void visit(CompilationUnitAST& Node) override {
    for (auto Declaration : Node.Declarations) {
      Declaration->accept(*this);
    }
  }

  void visit(VariableDeclarationAST& Node) override {

  }

  virtual void visit(FunctionDeclarationAST& Node) override {
  }

  virtual void visit(StatementSequenceAST& Node) override {
  }

  virtual void visit(IfStatementAST& Node) override {
  }

  virtual void visit(WhileStatementAST& Node) override {
  }

  virtual void visit(BreakStatementAST& Node) override {
  }

  virtual void visit(ContinueStatementAST& Node) override {
  }

  virtual void visit(ReturnStatementAST& Node) override {
  }

  virtual void visit(AssignStatementAST& Node) override {
  }

  virtual void visit(PrintStatementAST& Node) override {
  }

  virtual void visit(IntegerTypeAST&) override {
  }

  virtual void visit(ArrayTypeAST& Node) override {
  }

  virtual void visit(ArgumentsListAST&) override {
  }

  virtual void visit(ExpressionsListAST&) override {
  }

  void visit(ExpressionAST& Node) override {
  }

  void visit(RelationAST&) override {
  }

  void visit(SimpleExpressionAST& Node) override {
  }

  void visit(AddOperatorAST&) override {
  }

  void visit(TermAST& Node) override {
  }

  void visit(MulOperatorAST&) override {
  }

  void visit(MulOperandAST& Node) override {
  }

  void visit(UnaryOperatorAST&) override {
  }

  void visit(IdentifierAST& Node) override {
  }

  void visit(IntegerLiteralAST& Node) override {
  }

  void visit(ArrayInitializationAST& Node) override {
  }

  void visit(GetByIndexAST& Node) override {
  }

  void visit(ExpressionFactorAST& Node) override {
    Node.Expr->accept(*this);
  }

  void visit(FunctionCallAST& Node) override {
  }
};

char* ToBytecode::generate(AST& Ast) {
  return nullptr;
}

char* BytecodeGenerator::generate(AST&) {
  return nullptr;
}
