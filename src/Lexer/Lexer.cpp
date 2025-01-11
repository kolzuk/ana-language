#include "lexer/Lexer.h"

namespace charinfo {
LLVM_READNONE inline bool isWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\f' ||
      c == '\v' || c == '\r' || c == '\n';
}
LLVM_READNONE inline bool isDigit(char c) {
  return c >= '0' && c <= '9';
}
LLVM_READNONE inline bool isLetter(char c) {
  return (c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z');
}
}

void Lexer::next(Token& Token) {
  while (*BufferPtr && charinfo::isWhitespace(*BufferPtr))
    ++BufferPtr;

  while (*BufferPtr == '#') { // skip comments
    while (*BufferPtr && *BufferPtr != '\n') {
      ++BufferPtr;
    }
    while (*BufferPtr && charinfo::isWhitespace(*BufferPtr)) {
      ++BufferPtr;
    }
  }

  if (!*BufferPtr) {
    Token.Kind = TokenKind::EOI;
    return;
  }

  if (charinfo::isLetter(*BufferPtr)) {
    const char* End = BufferPtr + 1;
    while (charinfo::isLetter(*End) || charinfo::isDigit(*End) || *End == '_')
      ++End;
    llvm::StringRef Name(BufferPtr, End - BufferPtr);
    TokenKind Kind;
    if (Name == "if") {
      Kind = TokenKind::KW_if;
    } else if (Name == "else") {
      Kind = TokenKind::KW_else;
    } else if (Name == "for") {
      Kind = TokenKind::KW_for;
    } else if (Name == "while") {
      Kind = TokenKind::KW_while;
    } else if (Name == "return") {
      Kind = TokenKind::KW_return;
    } else if (Name == "integer") {
      Kind = TokenKind::KW_integer;
    } else if (Name == "bool") {
      Kind = TokenKind::KW_bool;
    } else if (Name == "true") {
      Kind = TokenKind::KW_true;
    } else if (Name == "false") {
      Kind = TokenKind::KW_false;
    } else {
      Kind = TokenKind::Identifier;
    }
    formToken(Token, End, Kind);
    return;
  }

  if (charinfo::isDigit(*BufferPtr)) {
    const char* End = BufferPtr + 1;
    while (charinfo::isDigit(*End))
      End++;
    formToken(Token, End, TokenKind::Number);
    return;
  }

  if (*BufferPtr == '"') {
    const char* End = BufferPtr + 1;
    while (*End && *End != '"')
      ++End;
    if (*End == '"') {
      ++BufferPtr;
      formToken(Token, End, String);
      BufferPtr = End + 1;
    } else {
      formToken(Token, BufferPtr + 1, Unknown);
    }
    return;
  }

  if (*BufferPtr == '=' && *(BufferPtr + 1) == '=') {
    formToken(Token, BufferPtr + 2, Equal);
  } else if (*BufferPtr == '<' && *(BufferPtr + 1) == '=') {
    formToken(Token, BufferPtr + 2, LessEq);
  } else if (*BufferPtr == '>' && *(BufferPtr + 1) == '=') {
    formToken(Token, BufferPtr + 2, GreaterEq);
  } else if (*BufferPtr == '!' && *(BufferPtr + 1) == '=') {
    formToken(Token, BufferPtr + 2, NotEqual);
  } else if (*BufferPtr == '|' && *(BufferPtr + 1) == '|') {
    formToken(Token, BufferPtr + 2, Or);
  } else if (*BufferPtr == '&' && *(BufferPtr + 1) == '&') {
    formToken(Token, BufferPtr + 2, And);
  } else {
    switch (*BufferPtr) {
#define CASE(ch, tok) \
case ch: formToken(Token, BufferPtr + 1, tok); break
      CASE('=', Assign);
      CASE('+', Plus);
      CASE('-', Minus);
      CASE('*', Star);
      CASE('/', Slash);
      CASE('%', Percent);
      CASE('<', Less);
      CASE('>', Greater);
      CASE('(', LParen);
      CASE(')', RParen);
      CASE('{', LFigure);
      CASE('}', RFigure);
      CASE('[', LSquare);
      CASE(']', RSquare);
      CASE(',', Comma);
      CASE(':', Colon);
      CASE(';', Semicolon);
#undef CASE
      default:formToken(Token, BufferPtr + 1, Unknown);
    }
    return;
  }
}

void Lexer::formToken(Token& Result,
                      const char* TokEnd,
                      TokenKind Kind) {
  Result.Kind = Kind;
  Result.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
  BufferPtr = TokEnd;
}
