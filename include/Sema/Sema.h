#ifndef SEMA_H
#define SEMA_H

#include "Parser/AST.h"
#include "Lexer/Lexer.h"
#include "llvm/ADT/StringSet.h"

class SemaVisitor : public ASTVisitor {
  llvm::StringSet<> Scope;
  bool HasError;

  enum ErrorType { Twice, Not };

  void error(ErrorType ET, llvm::StringRef V);

public:
  SemaVisitor() : HasError(false) {}
  bool hasError();

  void visit(AST& Node) override;
  void visit(CompilationUnitAST& Node) override;
  void visit(DeclarationAST& Node) override;
  void visit(VariableDeclarationAST& Node) override;
  void visit(FunctionDeclarationAST& Node) override;

  void visit(StatementSequenceAST& Node) override;
  void visit(StatementAST& Node) override;
  void visit(IfStatementAST& Node) override;
  void visit(WhileStatementAST& Node) override;
  void visit(ReturnStatementAST& Node) override;
  void visit(AssignStatementAST& Node) override;

  void visit(TypeAST& Node) override;
  void visit(IntegerTypeAST& Node) override;
  void visit(ArrayTypeAST& Node) override;
  void visit(ArgumentsListAST& Node) override;
  void visit(ExpressionsListAST& Node) override;

  void visit(ExpressionAST& Node) override;
  void visit(RelationAST& Node) override;
  void visit(SimpleExpressionAST& Node) override;
  void visit(AddOperatorAST& Node) override;
  void visit(TermAST& Node) override;
  void visit(MulOperatorAST& Node) override;
  void visit(MulOperandAST& Node) override;
  void visit(UnaryOperatorAST& Node) override;

  void visit(FactorAST& Node) override;
  void visit(IdentifierAST& Node) override;
  void visit(IntegerLiteralAST& Node) override;
  void visit(ArrayInitializationAST& Node) override;
  void visit(GetByIndexAST& Node) override;
  void visit(ExpressionFactorAST& Node) override;
  void visit(FunctionCallAST& Node) override;
};

class Sema {
  SemaVisitor Visitor;
 public:
  Sema() : Visitor() {}
  bool semantic(AST* Tree);
};

#endif //SEMA_H
