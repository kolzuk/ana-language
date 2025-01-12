#ifndef PARSER_H
#define PARSER_H

#include <llvm/ADT/StringMap.h>
#include <llvm/Support/raw_ostream.h>

#include "AST.h"
#include "Lexer/Lexer.h"

class Parser {
  Lexer& Lex;
  Token Tok;
  bool HasError;

  void error() {
    llvm::errs() << "Unexpected: " << Tok.getText() << "\n";
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

  AST* parseCompilationUnit();
  DeclarationAST* parseDeclaration();
  DeclarationAST* parseVariableDeclaration();
  AST* parseType();
  AST* parseExpression();
  AST* parseRelation();
  AST* parseSimpleExpression();
  AST* parseAddOperator();
  AST* parseTerm();
  AST* parseMulOperator();
  FactorAST* parseFactor();

 public:
  explicit Parser(Lexer& Lex) : Lex(Lex), HasError(false) {
    nextToken();
  }

  [[nodiscard]] bool hasError() const { return HasError; }
  AST* parse();
};

#endif //PARSER_H
