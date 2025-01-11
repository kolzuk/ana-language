#include "Parser/Parser.h"

AST* Parser::parse() {
  return parseStatementsSequence();
}

AST* Parser::parseStatementsSequence() {
  llvm::SmallVector<AST*, 8> Statements;
  while (!Tok.is(TokenKind::EOI)) {
    AST* Statement = parseStatement();
    Statements.push_back(Statement);
  }

  expect(TokenKind::EOI);

  return new StatementsSequence(Statements);
}

AST* Parser::parseStatement() {
  if (Tok.is(TokenKind::KW_integer)) {
    nextToken();
    llvm::StringRef Identifier = Tok.getText();
    nextToken();

    if (consume(TokenKind::Assign))
      goto _error;

    AST* E = parseExpr();

    if (consume(TokenKind::Semicolon))
      goto _error;

    return new Declaration(Identifier, E);
  } else {
    AST* E = parseExpr();

    if (consume(TokenKind::Semicolon))
      goto _error;

    return E;
  }

  _error:
  while (!Tok.is(TokenKind::EOI))
    nextToken();
  return nullptr;
}

AST* Parser::parseExpr() {
  AST* Left = parseTerm();

  while (Tok.isOneOf(TokenKind::Plus,
                     TokenKind::Minus,
                     TokenKind::Star,
                     TokenKind::Slash,
                     TokenKind::Assign)) {
    BinaryOp::Operator Op = BinaryOp::Div;
    switch (Tok.getKind()) {
      case (TokenKind::Plus):Op = BinaryOp::Plus;
        break;
      case (TokenKind::Minus):Op = BinaryOp::Plus;
        break;
      case (TokenKind::Star):Op = BinaryOp::Mul;
        break;
      case (TokenKind::Slash):Op = BinaryOp::Div;
        break;
      default:error();
    }

    nextToken();

    AST* Right = parseTerm();
    Left = new BinaryOp(Op, Left, Right);
  }

  return Left;
}

AST* Parser::parseTerm() {
  AST* Left = parseFactor();

  while (Tok.isOneOf(TokenKind::Star, TokenKind::Slash)) {
    BinaryOp::Operator Op =
        Tok.is(TokenKind::Star) ? BinaryOp::Mul : BinaryOp::Div;

    nextToken();

    AST* Right = parseFactor();
    Left = new BinaryOp(Op, Left, Right);
  }

  return Left;
}

AST* Parser::parseFactor() {
  AST* Res = nullptr;

  switch (Tok.getKind()) {
    case TokenKind::Number:Res = new Factor(Factor::Number, Tok.getText());
      nextToken();
      break;
    case TokenKind::Identifier:Res = new Factor(Factor::Ident, Tok.getText());
      nextToken();
      break;
    case TokenKind::LParen:nextToken();
      Res = parseExpr();
      if (!consume(TokenKind::RParen)) break;
    default:if (!Res) error();

      while (!Tok.isOneOf(TokenKind::RParen, TokenKind::Star,
                          TokenKind::Plus, TokenKind::Minus,
                          TokenKind::Slash, TokenKind::EOI))
        nextToken();
  }

  return Res;
}
