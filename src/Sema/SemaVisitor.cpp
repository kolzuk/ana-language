#include <llvm/Support/raw_ostream.h>

#include "Sema/Sema.h"
#include "Parser/AST.h"
#include "llvm/ADT/StringSet.h"

void SemaVisitor::error(SemaVisitor::ErrorType ET, llvm::StringRef V) {
  llvm::errs() << "Variable " << V << " "
             << (ET == Twice ? "already" : "not")
             << " declared\n";
  HasError = true;
}

bool SemaVisitor::hasError() { return HasError; }

void SemaVisitor::visit(AST& Node) { };
void SemaVisitor::visit(CompilationUnitAST& Node) {};
 void SemaVisitor::visit(DeclarationAST& Node) {};
void SemaVisitor::visit(VariableDeclarationAST& Node) {
  llvm::StringRef I = Node.getIdentifier();
  if (!Scope.insert(I).second)
    error(Twice, I);
  if (Node.getExpr())
    Node.getExpr()->accept(*this);
  else
    HasError = true;
};
void SemaVisitor::visit(FunctionDeclarationAST& Node) {}

void SemaVisitor::visit(StatementSequenceAST& Node) {};
void SemaVisitor::visit(StatementAST& Node) {};
void SemaVisitor::visit(IfStatementAST& Node) {};
void SemaVisitor::visit(WhileStatementAST& Node) {};
void SemaVisitor::visit(ReturnStatementAST& Node) {};
void SemaVisitor::visit(AssignStatementAST& Node) {}

void SemaVisitor::visit(TypeAST& Node) {};
void SemaVisitor::visit(IntegerTypeAST& Node) {};
void SemaVisitor::visit(ArrayTypeAST& Node) {};
void SemaVisitor::visit(ArgumentsListAST& Node) {};
void SemaVisitor::visit(ExpressionsListAST& Node) {}

void SemaVisitor::visit(ExpressionAST& Node) {};
void SemaVisitor::visit(RelationAST& Node) {};
void SemaVisitor::visit(SimpleExpressionAST& Node) {};
void SemaVisitor::visit(AddOperatorAST& Node) {
  if (Node.getLeft())
    Node.getLeft()->accept(*this);
  else
    HasError = true;
  if (Node.getRight())
    Node.getRight()->accept(*this);
  else
    HasError = true;
};
void SemaVisitor::visit(TermAST& Node) {};
void SemaVisitor::visit(MulOperatorAST& Node) {};
void SemaVisitor::visit(MulOperandAST& Node) {};
void SemaVisitor::visit(UnaryOperatorAST& Node) {}

void SemaVisitor::visit(FactorAST& Node) {
  if (Node.getKind() == Factor::Ident) {
    if (Scope.find(Node.getVal()) == Scope.end())
      error(Not, Node.getVal());
  }
};

void SemaVisitor::visit(IdentifierAST& Node) {};
void SemaVisitor::visit(IntegerLiteralAST& Node) {};
void SemaVisitor::visit(ArrayInitializationAST& Node) {};
void SemaVisitor::visit(GetByIndexAST& Node) {};
void SemaVisitor::visit(ExpressionFactorAST& Node) {};
void SemaVisitor::visit(FunctionCallAST& Node) {};