#include "Parser/Parser.h"

AST* Parser::parse() {
  return parseCompilationUnit();
}

/// compilationUnit : ( declaration )*;
CompilationUnitAST* Parser::parseCompilationUnit() {
  llvm::SmallVector<DeclarationAST*, 8> Declarations;
  while (!Tok.is(TokenKind::EOI)) {
    DeclarationAST* Declaration = parseDeclaration();
    Declarations.push_back(Declaration);
  }

  consume(TokenKind::EOI);

  return new CompilationUnitAST(Declarations);
}

/// declaration : ( variableDeclaration | functionDeclaration );
DeclarationAST* Parser::parseDeclaration() {
  if (Tok.is(TokenKind::KW_fun)) {
    return parseFunctionDeclaration();
  } else {
    return parseVariableDeclaration();
  }
}

/// variableDeclaration : type identifier ( "=" expression ) ";";
VariableDeclarationAST* Parser::parseVariableDeclaration() {
  auto* T = parseType();
  auto* I = parseIdentifier();
  consume(TokenKind::Assign);
  auto* E = parseExpression();
  consume(TokenKind::Semicolon);
  return new VariableDeclarationAST(T, I, E);
}

/// "fun" identifier "(" argumentList? ")" ( "->" type )? "{" statementSequence "}"
FunctionDeclarationAST* Parser::parseFunctionDeclaration() {
  consume(TokenKind::KW_fun);
  auto* Ident = parseIdentifier();
  consume(TokenKind::LParen);
  auto* ArgList = parseArgumentsList();
  consume(TokenKind::RParen);
  TypeAST* Type;
  if (Tok.is(TokenKind::Arrow)) {
    nextToken();
    Type = parseType();
  } else {
    Type = new VoidTypeAST();
  }
  consume(TokenKind::LFigure);
  auto* SS = parseStatementSequence();
  consume(TokenKind::RFigure);
  return new FunctionDeclarationAST(Type, Ident, ArgList, SS);
}

/// type : "integer" | "array" "[" expression "]";
TypeAST* Parser::parseType() {
  if (Tok.is(TokenKind::KW_integer)) {
    nextToken();
    return new IntegerTypeAST();
  }
  if (Tok.is(TokenKind::KW_array)) {
    nextToken();
    consume(TokenKind::LSquare);
    auto* Size = new IntegerLiteralAST(Tok.getText());
    nextToken();
    consume(TokenKind::RSquare);
    return new ArrayTypeAST(Size);
  }
  error();
  return nullptr;
}

/// expression : simpleExpression (relation simpleExpression)?;
ExpressionAST* Parser::parseExpression() {
  auto Left = parseSimpleExpression();

  if (Tok.isOneOf(TokenKind::Equal,
                  TokenKind::NotEqual,
                  TokenKind::Less,
                  TokenKind::LessEq,
                  TokenKind::Greater,
                  TokenKind::GreaterEq)) {
    auto Rel = parseRelation();
    auto Right = parseSimpleExpression();
    return new ExpressionAST(Left, Rel, Right);
  }

  return new ExpressionAST(Left, nullptr, nullptr);
}

/// relation : "==" | "!=" | "<" | "<=" | ">" | ">=";
RelationAST* Parser::parseRelation() {
  RelationAST* Rel = nullptr;
  switch (Tok.getKind()) {
    case (TokenKind::Equal):Rel = new RelationAST(RelationAST::Equal);
      break;
    case (TokenKind::NotEqual):Rel = new RelationAST(RelationAST::NotEqual);
      break;
    case (TokenKind::Less):Rel = new RelationAST(RelationAST::Less);
      break;
    case (TokenKind::LessEq):Rel = new RelationAST(RelationAST::LessEq);
      break;
    case (TokenKind::Greater):Rel = new RelationAST(RelationAST::Greater);
      break;
    case (TokenKind::GreaterEq):Rel = new RelationAST(RelationAST::GreaterEq);
      break;
    default:error();
  }

  nextToken();
  return Rel;
}

/// simpleExpression : term (addOperator term)*;
SimpleExpressionAST* Parser::parseSimpleExpression() {
  auto* FirstTrm = parseTerm();
  llvm::SmallVector<AddOperatorAST*> AddOperators;
  llvm::SmallVector<TermAST*> Terms;

  while (Tok.isOneOf(TokenKind::Plus, TokenKind::Minus)) {
    auto AddOper = parseAddOperator();
    auto* Trm = parseTerm();
    AddOperators.push_back(AddOper);
    Terms.push_back(Trm);
  }

  return new SimpleExpressionAST(FirstTrm, AddOperators, Terms);
}

/// addOperator : "+" | "-";
AddOperatorAST* Parser::parseAddOperator() {
  AddOperatorAST* AddOper = nullptr;
  switch (Tok.getKind()) {
    case (TokenKind::Plus):AddOper = new AddOperatorAST(AddOperatorAST::Plus);
      break;
    case (TokenKind::Minus):AddOper = new AddOperatorAST(AddOperatorAST::Minus);
      break;
    default:error();
  }

  nextToken();
  return AddOper;
}

/// term : factor (mulOperator factor)*;
TermAST* Parser::parseTerm() {
  auto* FirstFactor = parseMulOperand();
  llvm::SmallVector<MulOperatorAST*> MulOperators;
  llvm::SmallVector<MulOperandAST*> Factors;

  while (Tok.isOneOf(TokenKind::Star, TokenKind::Slash)) {
    auto* MulOper = parseMulOperator();
    auto* Factor = parseMulOperand();
    MulOperators.push_back(MulOper);
    Factors.push_back(Factor);
  }

  return new TermAST(FirstFactor, MulOperators, Factors);
}

/// mulOperator : "*" | "/";
MulOperatorAST* Parser::parseMulOperator() {
  MulOperatorAST* MulOper = nullptr;
  switch (Tok.getKind()) {
    case (TokenKind::Star):MulOper = new MulOperatorAST(MulOperatorAST::Multiple);
      break;
    case (TokenKind::Slash):MulOper = new MulOperatorAST(MulOperatorAST::Divide);
      break;
    default:error();
  }

  nextToken();
  return MulOper;
}

/// factor
///   : integer_literal
///    | arrayInitialization
///    | identifier
///    | getByIndex
///    | expressionFactor
///    | functionCall
FactorAST* Parser::parseFactor() {
  if (Tok.is(TokenKind::Number)) {
    auto* Number = new IntegerLiteralAST(Tok.getText());
    nextToken();
    return Number;
  }

  if (Tok.is(TokenKind::Identifier)) {
    if (Lex.peek(0).is(TokenKind::LParen)) {
      return parseFunctionCall();
    }
    if (Lex.peek(0).is(TokenKind::LSquare)) {
      return parseGetByIndex();
    }
    return parseIdentifier();
  }

  if (Tok.is(TokenKind::LParen)) {
    nextToken();
    ExpressionAST* Expr = parseExpression();
    consume(TokenKind::RParen);
    return new ExpressionFactorAST(Expr);
  }

  if (Tok.is(TokenKind::LSquare)) {
    return parseArrayInitialization();
  }

  error();
  return nullptr;
}

/// mulOperand : unaryOperator? factor
MulOperandAST* Parser::parseMulOperand() {
  UnaryOperatorAST* Operator = nullptr;
  if (Tok.isOneOf(TokenKind::Plus, TokenKind::Minus)) {
    Operator = parseUnaryOperator();
  }
  auto* Factor = parseFactor();
  return new MulOperandAST(Factor, Operator);
}

/// unaryOperator : ("+" | "-")
UnaryOperatorAST* Parser::parseUnaryOperator() {
  UnaryOperatorAST* Operator = nullptr;
  switch (Tok.getKind()) {
    case (TokenKind::Plus):Operator = new UnaryOperatorAST(UnaryOperatorAST::Plus);
      break;
    case (TokenKind::Minus):Operator = new UnaryOperatorAST(UnaryOperatorAST::Minus);
      break;
    default:error();
  }

  nextToken();
  return Operator;
}

/// identfier
IdentifierAST* Parser::parseIdentifier() {
  if (Tok.is(TokenKind::Identifier)) {
    auto* I = new IdentifierAST(Tok.getText());
    nextToken();
    return I;
  }
  error();
  return nullptr;
}

/// statementSequence : statement*
StatementSequenceAST* Parser::parseStatementSequence() {
  std::vector<StatementAST*> Statements;
  while (!Tok.is(TokenKind::RFigure)) {
    auto* Statement = parseStatement();
    Statements.push_back(Statement);
  }
  return new StatementSequenceAST(Statements);
}

/// statement
/// : variableDeclaration
/// | ifStatement
/// | whileStatement
/// | returnStatement
/// | (assignStatement ";")
StatementAST* Parser::parseStatement() {
  if (Tok.is(TokenKind::KW_if)) {
    return parseIfStatement();
  }
  if (Tok.is(TokenKind::KW_while)) {
    return parseWhileStatement();
  }
  if (Tok.is(TokenKind::KW_return)) {
    return parseReturnStatement();
  }
  if (Tok.isOneOf(TokenKind::KW_integer, TokenKind::KW_array)) {
    return parseVariableDeclaration();
  }
  auto* Statement = parseAssignStatement();
  consume(TokenKind::Semicolon);
  return Statement;
}

/// assignStatement : expression ( "=" expression)?
AssignStatementAST* Parser::parseAssignStatement() {
  auto* LHS = parseExpression();
  ExpressionAST* RHS = nullptr;
  if (Tok.is(TokenKind::Assign)) {
    nextToken();
    RHS = parseExpression();
  }
  return new AssignStatementAST(LHS, RHS);
}

/// argumentList : ( type identifier ( "," type identifier )* )?
ArgumentsListAST* Parser::parseArgumentsList() {
  llvm::SmallVector<IdentifierAST*> Idents;
  llvm::SmallVector<TypeAST*> Types;
  if (Tok.is(TokenKind::RParen)) {
    return new ArgumentsListAST(Idents, Types);
  } else {
    auto* T = parseType();
    auto* I = parseIdentifier();
    Types.push_back(T);
    Idents.push_back(I);
  }
  while (Tok.is(TokenKind::Comma)) {
    nextToken();
    auto* T = parseType();
    auto* I = parseIdentifier();
    Types.push_back(T);
    Idents.push_back(I);
  }
  return new ArgumentsListAST(Idents, Types);
}

/// expressionList : ( expression ( "," expression )* )? ;
ExpressionsListAST* Parser::parseExpressionsList() {
  llvm::SmallVector<ExpressionAST*> Exprs;
  if (Tok.is(TokenKind::RParen)) {
    return new ExpressionsListAST(Exprs);
  } else {
    auto* E = parseExpression();
    Exprs.push_back(E);
  }
  while (Tok.is(TokenKind::Comma)) {
    nextToken();
    auto* E = parseExpression();
    Exprs.push_back(E);
  }
  return new ExpressionsListAST(Exprs);
}

/// arrayInitialization : "[" (expression ("," expression)*) "]";
ArrayInitializationAST* Parser::parseArrayInitialization() {
  consume(TokenKind::LSquare);
  llvm::SmallVector<ExpressionAST*> Exprs;
  auto* E = parseExpression();
  Exprs.push_back(E);

  while (Tok.is(TokenKind::Comma)) {
    nextToken();
    E = parseExpression();
    Exprs.push_back(E);
  }
  consume(TokenKind::RSquare);
  return new ArrayInitializationAST(Exprs);
}

/// getByIndex : identifier "[" expression "]";
GetByIndexAST* Parser::parseGetByIndex() {
  auto* I = parseIdentifier();
  consume(TokenKind::LSquare);
  auto* Expr = parseExpression();
  consume(TokenKind::RSquare);
  return new GetByIndexAST(I, Expr);
}

/// functionCall : identifier "(" expressionList ")" ;
FunctionCallAST* Parser::parseFunctionCall() {
  auto* I = parseIdentifier();
  consume(TokenKind::LParen);
  auto* ExprList = parseExpressionsList();
  consume(TokenKind::RParen);
  return new FunctionCallAST(I, ExprList);
}

/// ifStatement
///    : "if" "(" expression ")" "{" statementSequence "}"
///    ( "else" "{" statementSequence "}" )?
IfStatementAST* Parser::parseIfStatement() {
  consume(TokenKind::KW_if);
  consume(TokenKind::LParen);
  auto* Expr = parseExpression();
  consume(TokenKind::RParen);
  consume(TokenKind::LFigure);
  auto* SS = parseStatementSequence();
  consume(TokenKind::RFigure);

  if (Tok.is(TokenKind::KW_else)) {
    nextToken();
    consume(TokenKind::LFigure);
    auto* ElseSS = parseStatementSequence();
    consume(TokenKind::RFigure);
    return new IfStatementAST(Expr, SS, ElseSS);
  }

  return new IfStatementAST(Expr, SS, nullptr);
}

/// whileStatement : "while" "(" expression ")" "{" statementSequence "}"
WhileStatementAST* Parser::parseWhileStatement() {
  consume(TokenKind::KW_while);
  consume(TokenKind::LParen);
  auto* Expr = parseExpression();
  consume(TokenKind::RParen);
  consume(TokenKind::LFigure);
  auto* SS = parseStatementSequence();
  consume(TokenKind::RFigure);

  return new WhileStatementAST(Expr, SS);
}

/// returnStatement : "return" ( expression )? ";"
ReturnStatementAST* Parser::parseReturnStatement() {
  consume(TokenKind::KW_return);
  if (Tok.is(TokenKind::Semicolon)) {
    nextToken();
    return new ReturnStatementAST(nullptr);
  }
  auto* Expr = parseExpression();
  consume(TokenKind::Semicolon);
  return new ReturnStatementAST(Expr);
}

