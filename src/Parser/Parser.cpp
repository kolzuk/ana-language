#include "Parser/Parser.h"

AST* Parser::parse() {
  return parseCompilationUnit();
}

AST* Parser::parseCompilationUnit() {
  llvm::SmallVector<AST*, 8> Declarations;
  while (!Tok.is(TokenKind::EOI)) {
    AST* Declaration = parseDeclaration();
    Declarations.push_back(Declaration);
  }

  expect(TokenKind::EOI);

  return new StatementsSequence(Declarations);
}

AST* Parser::parseDeclaration() {
  // TODO
  if (Tok.is(TokenKind::KW_fun)) {

  } else {
    return parseVariableDeclaration();
  }
}

AST* Parser::parseVariableDeclaration() {
  auto* T = dynamic_cast<Type*>(parseType());
  auto* I = dynamic_cast<Identifier*>(parseIdentifier());
  auto* E = dynamic_cast<Expression*>(parseExpression());

  return new VariableDeclaration(T, I, E);
}

AST* Parser::parseType() {
  if (Tok.is(TokenKind::KW_integer)) {
    nextToken();
    return new Type(Type::Integer, nullptr);
  } else if (Tok.is(TokenKind::KW_array)) {
    nextToken();
    Type* NextType = dynamic_cast<Type*>(parseType());
    return new Type(Type::Array, NextType);
  } else {
    error();
  }
}

AST* Parser::parseStatementsSequence() {
  llvm::SmallVector<AST*, 8> Statements;

  auto Stmt = dynamic_cast<Statement*>(parseStatement());
  Statements.push_back(Stmt);
  while (!Tok.is(TokenKind::Semicolon)) {
    nextToken();
    Stmt = dynamic_cast<Statement*>(parseStatement());
    Statements.push_back(Stmt);
  }

  return new StatementsSequence(Statements);
}

// TODO
AST* Parser::parseStatement() {
  if (Tok.is(TokenKind::KW_integer) || Tok.is(TokenKind::KW_array)) {
    return parseVariableDeclaration();
  } else if (Tok.is(TokenKind::Identifier)) {
  }
}

AST* Parser::parseRelation() {
  Relation* Rel;
  switch (Tok.getKind()) {
    case (TokenKind::Equal):Rel = new Relation(Relation::Equal); break;
    case (TokenKind::NotEqual):Rel = new Relation(Relation::NotEqual); break;
    case (TokenKind::Less):Rel = new Relation(Relation::Less); break;
    case (TokenKind::LessEq):Rel = new Relation(Relation::LessEq); break;
    case (TokenKind::Greater):Rel = new Relation(Relation::Greater); break;
    case (TokenKind::GreaterEq):Rel = new Relation(Relation::GreaterEq); break;
    default:error();
  }

  nextToken();
  return Rel;
}

AST* Parser::parseExpression() {
  auto Left = dynamic_cast<SimpleExpression*>(parseSimpleExpression());
  nextToken();

  if (Tok.isOneOf(TokenKind::Equal,
                     TokenKind::NotEqual,
                     TokenKind::Less,
                     TokenKind::LessEq,
                     TokenKind::Greater,
                     TokenKind::GreaterEq)) {
    auto Rel = dynamic_cast<Relation*>(parseRelation());
    auto Right = dynamic_cast<SimpleExpression*>(parseSimpleExpression());
    return new Expression(Left, Rel, Right);
  }

  return new Expression(Left, nullptr, nullptr);
}

AST *Parser::parseSimpleExpression() {
  SimpleExpression::SignId SignKind;
  if (Tok.is(TokenKind::Plus)) {
    SignKind = SimpleExpression::SignId::Plus;
    nextToken();
  } else if (Tok.is(TokenKind::Minus)) {
    SignKind = SimpleExpression::SignId::Minus;
    nextToken();
  }

  auto* FirstTrm = dynamic_cast<Term*>(parseTerm());

  llvm::SmallVector<AddOperator*> AddOperators;
  llvm::SmallVector<Term*> Terms;

  while (Tok.isOneOf(TokenKind::Plus, TokenKind::Minus)) {
    auto AddOper = dynamic_cast<AddOperator*>(parseAddOperator());
    auto* Trm = dynamic_cast<Term*>(parseTerm());
    AddOperators.push_back(AddOper);
    Terms.push_back(Trm);
  }

  return new SimpleExpression(SignKind, FirstTrm, AddOperators, Terms);
}

AST *Parser::parseAddOperator() {
  AddOperator* AddOper;
  switch (Tok.getKind()) {
    case (TokenKind::Plus):AddOper = new AddOperator(AddOperator::Plus); break;
    case (TokenKind::Minus):AddOper = new AddOperator(AddOperator::Minus); break;
    default:error();
  }

  nextToken();
  return AddOper;
}

AST* Parser::parseTerm() {
  auto* FirstFactor = dynamic_cast<Factor*>(parseFactor());

  llvm::SmallVector<MulOperator*> MulOperators;
  llvm::SmallVector<Factor*> Factors;

  while (Tok.isOneOf(TokenKind::Star, TokenKind::Slash)) {
    auto MulOper = dynamic_cast<MulOperator*>(parseMulOperator());
    auto* Fctor = dynamic_cast<Factor*>(parseFactor());
    MulOperators.push_back(MulOper);
    Factors.push_back(Fctor);
  }

  return new Term(FirstFactor, MulOperators, Factors);
}

AST* Parser::parseMulOperator() {
  MulOperator* MulOper;
  switch (Tok.getKind()) {
    case (TokenKind::Star):MulOper = new MulOperator(MulOperator::Multiple); break;
    case (TokenKind::Slash):MulOper = new MulOperator(MulOperator::Divide); break;
    default:error();
  }

  nextToken();
  return MulOper;
}

AST* Parser::parseFactor() {
  AST* Res;
  if (Tok.is(TokenKind::Number))
    return parseIntegerLiteral();
  else if (Tok.is(TokenKind::Identifier)) {
    if (Tok.is(TokenKind::LParen)) {
      Res = parseFunctionCall();
      nextToken();
      consume(TokenKind::RParen);
    } else if (Tok.is(TokenKind::LSquare)) {
      Res = parseGetByIndex();
      nextToken();
      consume(TokenKind::RSquare);
    } else {
      Res = parseIdentifier();
    }

    return Res;
  } if (Tok.is(TokenKind::LParen)) {
    nextToken();
    Res = parseExpression();
    consume(TokenKind::RParen);
    return Res;
  }

  error();
  return nullptr;
}

AST* Parser::parseFunctionCall() {

}

AST *Parser::parseExpressionList() {
  return nullptr;
}

AST* parseGetByIndex() {

}

AST* Parser::parseIdentifier() {
  if (Tok.is(TokenKind::Identifier)) {
    nextToken();
    return new Identifier(Tok.getText());
  } else {
    error();
  }

  return nullptr;
}

AST* Parser::parseIntegerLiteral() {
  if (Tok.is(TokenKind::Number)) {
    nextToken();
    return new IntegerLiteral(std::stoll(Tok.getText().str()));
  } else {
    error();
  }

  return nullptr;
}

