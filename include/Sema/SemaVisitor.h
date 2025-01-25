#ifndef SEMAVISITOR_H
#define SEMAVISITOR_H

#include "Parser/AST.h"
#include "Sema/Scope.h"

class SemaVisitor : public ASTVisitor {
  Scope* Scp;
  TypeAST::TypeKind LastType;
  size_t LastArraySize;
  std::string LastFunctionName;
  bool HasError;

  void error(const std::string& Message);

  static std::string typeToString(TypeAST::TypeKind Type);
  static std::string generateAlreadyMessage(const std::string& Name);
  static std::string generateNotDeclaredMessage(const std::string& Name);
  static std::string generateNotMatchTypeMessage(const std::string& Name, TypeAST::TypeKind Type);
  static std::string generateNotMatchSizeMessage(const std::string& Name, size_t Size);
  static std::string generateWrongArgumentsAmountMessage(const std::string& Name, size_t Size);
  static std::string generateWrongGetByIndexMessage(const std::string& Name);
  static std::string generateNotIntegerIndexMessage();
  static std::string generateNotIntegerExpressionMessage();
public:
  SemaVisitor() : Scp(nullptr), LastType(TypeAST::Void), LastArraySize(0), HasError(false) {}

  bool hasError();
  ArgumentsListAST* getArguments();

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
  void visit(PrintStatementAST& Node) override;
  void visit(BreakStatementAST& Node) override;
  void visit(ContinueStatementAST& Node) override;

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

#endif //SEMAVISITOR_H
