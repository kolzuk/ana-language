#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include "AST.h"
#include "Lexer/Lexer.h"

class Parser {
  Lexer& Lex;
  Token Tok;
  bool HasError;

  void error() {
    std::cerr << "[line " << Tok.getLine() << ", column " << Tok.getColumn() << "] Unexpected: " << Tok.getText() << "\n";
    HasError = true;
  }

  void nextToken() { Lex.next(Tok); }

  bool expect(TokenKind Kind) {
    if (Tok.getKind() != Kind) {
      error();
      return true;
    }

    return false;
  }

  bool consume(TokenKind Kind) {
    if (expect(Kind))
      return true;
    nextToken();
    return false;
  }

  CompilationUnitAST* parseCompilationUnit();
  DeclarationAST* parseDeclaration();
  VariableDeclarationAST* parseVariableDeclaration();
  FunctionDeclarationAST* parseFunctionDeclaration();

  StatementSequenceAST* parseStatementSequence();
  StatementAST* parseStatement();
  AssignStatementAST* parseAssignStatement();

  IfStatementAST* parseIfStatement();
  WhileStatementAST* parseWhileStatement();
  ReturnStatementAST* parseReturnStatement();
  PrintStatementAST* parsePrintStatement();
  BreakStatementAST* parseBreakStatement();
  ContinueStatementAST* parseContinueStatement();

  ArgumentsListAST* parseArgumentsList();
  ExpressionsListAST* parseExpressionsList();

  TypeAST* parseType();
  ExpressionAST* parseExpression();
  RelationAST* parseRelation();
  SimpleExpressionAST* parseSimpleExpression();
  AddOperatorAST* parseAddOperator();
  TermAST* parseTerm();
  MulOperatorAST* parseMulOperator();
  MulOperandAST* parseMulOperand();
  UnaryOperatorAST* parseUnaryOperator();

  FactorAST* parseFactor();
  ArrayInitializationAST* parseArrayInitialization();
  GetByIndexAST* parseGetByIndex();
  FunctionCallAST* parseFunctionCall();
  IdentifierAST* parseIdentifier();

 public:
  explicit Parser(Lexer& Lex) : Lex(Lex), HasError(false) {
    nextToken();
  }

  [[nodiscard]] bool hasError() const { return HasError; }
  AST* parse();
};

#endif //PARSER_H