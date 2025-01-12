#include "Lexer/Lexer.h"

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
      formToken(Token, End, TokenKind::Identifier);
      BufferPtr = End + 1;
    } else {
      formToken(Token, BufferPtr + 1, TokenKind::Unknown);
    }
    return;
  }

  if (*BufferPtr == '=' && *(BufferPtr + 1) == '=') {
    formToken(Token, BufferPtr + 2, TokenKind::Equal);
  } else if (*BufferPtr == '<' && *(BufferPtr + 1) == '=') {
    formToken(Token, BufferPtr + 2, TokenKind::LessEq);
  } else if (*BufferPtr == '>' && *(BufferPtr + 1) == '=') {
    formToken(Token, BufferPtr + 2, TokenKind::GreaterEq);
  } else if (*BufferPtr == '!' && *(BufferPtr + 1) == '=') {
    formToken(Token, BufferPtr + 2, TokenKind::NotEqual);
  } else if (*BufferPtr == '|' && *(BufferPtr + 1) == '|') {
    formToken(Token, BufferPtr + 2, TokenKind::Or);
  } else if (*BufferPtr == '&' && *(BufferPtr + 1) == '&') {
    formToken(Token, BufferPtr + 2, TokenKind::And);
  } else {
    switch (*BufferPtr) {
#define CASE(ch, tok) \
case ch: formToken(Token, BufferPtr + 1, tok); break
      CASE('=', TokenKind::Assign);
      CASE('+', TokenKind::Plus);
      CASE('-', TokenKind::Minus);
      CASE('*', TokenKind::Star);
      CASE('/', TokenKind::Slash);
      CASE('%', TokenKind::Percent);
      CASE('<', TokenKind::Less);
      CASE('>', TokenKind::Greater);
      CASE('(', TokenKind::LParen);
      CASE(')', TokenKind::RParen);
      CASE('{', TokenKind::LFigure);
      CASE('}', TokenKind::RFigure);
      CASE('[', TokenKind::LSquare);
      CASE(']', TokenKind::RSquare);
      CASE(',', TokenKind::Comma);
      CASE(':', TokenKind::Colon);
      CASE(';', TokenKind::Semicolon);
#undef CASE
      default:formToken(Token, BufferPtr + 1, TokenKind::Unknown);
    }
    return;
  }
}

// TODO
Token Lexer::peek(int n) {
  return Token();
}

void Lexer::formToken(Token& Result,
                      const char* TokEnd,
                      TokenKind Kind) {
  Result.Kind = Kind;
  Result.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
  BufferPtr = TokEnd;
}
