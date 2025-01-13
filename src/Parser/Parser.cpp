#include "Parser/Parser.h"

AST* Parser::parse() {
  return parseCompilationUnit();
}

/// compilationUnit : ( declaration )*;
AST* Parser::parseCompilationUnit() {
  llvm::SmallVector<DeclarationAST*, 8> Declarations;
  while (!Tok.is(TokenKind::EOI)) {
    DeclarationAST* Declaration = parseDeclaration();
    Declarations.push_back(Declaration);
  }

  consume(TokenKind::EOI);

  return new CompilationUnitAST(Declarations);
}

/// declaration : ( variableDeclaration );
DeclarationAST* Parser::parseDeclaration() {
  // TODO
  if (Tok.is(TokenKind::KW_fun)) {

  } else {
    return parseVariableDeclaration();
  }
}

/// variableDeclaration : type identifier ( "=" expression ) ";";
DeclarationAST* Parser::parseVariableDeclaration() {
  auto* T = dynamic_cast<TypeAST*>(parseType());
  auto* I = new IdentifierAST(Tok.getText());
  nextToken();
  consume(TokenKind::Assign);
  auto* E = dynamic_cast<ExpressionAST*>(parseExpression());
  consume(TokenKind::Semicolon);

  return new ConstantDeclaration(T, I, E);
}

/// type : "integer" | "array" "[" expression "]";
AST* Parser::parseType() {
  if (Tok.is(TokenKind::KW_integer)) {
    nextToken();
    return new IntegerTypeAST();
  }
  if (Tok.is(TokenKind::KW_array)) {
    nextToken();
    consume(TokenKind::LSquare);
    auto* Size = dynamic_cast<ExpressionAST*>(parseExpression());
    consume(TokenKind::RSquare);
    return new ArrayTypeAST(Size);
  }
  error();
  return nullptr;
}

/// expression : simpleExpression (relation simpleExpression)?;
AST* Parser::parseExpression() {
  auto Left = dynamic_cast<SimpleExpressionAST*>(parseSimpleExpression());
  nextToken();

  if (Tok.isOneOf(TokenKind::Equal,
                  TokenKind::NotEqual,
                  TokenKind::Less,
                  TokenKind::LessEq,
                  TokenKind::Greater,
                  TokenKind::GreaterEq)) {
    auto Rel = dynamic_cast<RelationAST*>(parseRelation());
    auto Right = dynamic_cast<SimpleExpressionAST*>(parseSimpleExpression());
    return new ExpressionAST(Left, Rel, Right);
  }

  return new ExpressionAST(Left, nullptr, nullptr);
}

/// relation : "==" | "!=" | "<" | "<=" | ">" | ">=";
AST* Parser::parseRelation() {
  RelationAST* Rel;
  switch (Tok.getKind()) {
    case (TokenKind::Equal):Rel = new RelationAST(RelationAST::Equal); break;
    case (TokenKind::NotEqual):Rel = new RelationAST(RelationAST::NotEqual); break;
    case (TokenKind::Less):Rel = new RelationAST(RelationAST::Less); break;
    case (TokenKind::LessEq):Rel = new RelationAST(RelationAST::LessEq); break;
    case (TokenKind::Greater):Rel = new RelationAST(RelationAST::Greater); break;
    case (TokenKind::GreaterEq):Rel = new RelationAST(RelationAST::GreaterEq); break;
    default:error();
  }

  nextToken();
  return Rel;
}

/// simpleExpression : ("+" | "-")? term (addOperator term)*;
AST *Parser::parseSimpleExpression() {
  SimpleExpressionAST::SignId SignKind;
  if (Tok.is(TokenKind::Plus)) {
    SignKind = SimpleExpressionAST::SignId::Plus;
    nextToken();
  } else if (Tok.is(TokenKind::Minus)) {
    SignKind = SimpleExpressionAST::SignId::Minus;
    nextToken();
  }

  auto* FirstTrm = dynamic_cast<TermAST*>(parseTerm());

  llvm::SmallVector<AddOperatorAST*> AddOperators;
  llvm::SmallVector<TermAST*> Terms;

  while (Tok.isOneOf(TokenKind::Plus, TokenKind::Minus)) {
    auto AddOper = dynamic_cast<AddOperatorAST*>(parseAddOperator());
    auto* Trm = dynamic_cast<TermAST*>(parseTerm());
    AddOperators.push_back(AddOper);
    Terms.push_back(Trm);
  }

  return new SimpleExpressionAST(SignKind, FirstTrm, AddOperators, Terms);
}

/// addOperator : "+" | "-";
AST *Parser::parseAddOperator() {
  AddOperatorAST* AddOper;
  switch (Tok.getKind()) {
    case (TokenKind::Plus):AddOper = new AddOperatorAST(AddOperatorAST::Plus); break;
    case (TokenKind::Minus):AddOper = new AddOperatorAST(AddOperatorAST::Minus); break;
    default:error();
  }

  nextToken();
  return AddOper;
}

/// term : factor (mulOperator factor)*;
AST* Parser::parseTerm() {
  auto* FirstFactor = dynamic_cast<FactorAST*>(parseFactor());

  llvm::SmallVector<MulOperatorAST*> MulOperators;
  llvm::SmallVector<FactorAST*> Factors;

  while (Tok.isOneOf(TokenKind::Star, TokenKind::Slash)) {
    auto MulOper = dynamic_cast<MulOperatorAST*>(parseMulOperator());
    auto* Fctor = dynamic_cast<FactorAST*>(parseFactor());
    MulOperators.push_back(MulOper);
    Factors.push_back(Fctor);
  }

  return new TermAST(FirstFactor, MulOperators, Factors);
}

///mulOperator : "*" | "/";
AST* Parser::parseMulOperator() {
  MulOperatorAST* MulOper;
  switch (Tok.getKind()) {
    case (TokenKind::Star):MulOper = new MulOperatorAST(MulOperatorAST::Multiple); break;
    case (TokenKind::Slash):MulOper = new MulOperatorAST(MulOperatorAST::Divide); break;
    default:error();
  }

  nextToken();
  return MulOper;
}

/// factor
///   : integer_literal
///   | identifier
///   | getByIndex
///   | expressionFactor
FactorAST* Parser::parseFactor() {
  FactorAST *Res = nullptr;
  if (Tok.is(TokenKind::Number))
    return new IntegerLiteralAST(Tok.getText());
  else if (Tok.is(TokenKind::Identifier)) {
    auto Name = new IdentifierAST(Tok.getText());
    nextToken();
    if (Tok.is(TokenKind::LSquare)) {
      nextToken();
      ExpressionAST* Expr = dynamic_cast<ExpressionAST*>(parseExpression());
      consume(TokenKind::RSquare);
      return new GetByIndexAST(Name, Expr);
    } else {
      return Name;
    }
  } else if (Tok.is(TokenKind::LParen)) {
    nextToken();
    ExpressionAST* Expr = dynamic_cast<ExpressionAST*>(parseExpression());
    consume(TokenKind::RParen);
    return new ExpressionFactorAST(Expr);
  }

  error();
  return nullptr;
}