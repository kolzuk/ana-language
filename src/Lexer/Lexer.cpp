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

void Lexer::next(Token& token) {
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
    token.Kind = TokenKind::EOI;
    return;
  }

  if (charinfo::isLetter(*BufferPtr)) {
    const char* end = BufferPtr + 1;
    while (charinfo::isLetter(*end) || charinfo::isDigit(*end) || *end == '_')
      ++end;
    llvm::StringRef name(BufferPtr, end - BufferPtr);
    TokenKind kind;
    if (name == "if") {
      kind = TokenKind::KW_if;
    } else if (name == "else") {
      kind = TokenKind::KW_else;
    } else if (name == "for") {
      kind = TokenKind::KW_for;
    } else if (name == "while") {
      kind = TokenKind::KW_while;
    } else if (name == "return") {
      kind = TokenKind::KW_return;
    } else if (name == "integer") {
      kind = TokenKind::KW_integer;
    } else if (name == "bool") {
      kind = TokenKind::KW_bool;
    } else if (name == "true") {
      kind = TokenKind::KW_true;
    } else if (name == "false") {
      kind = TokenKind::KW_false;
    } else {
      kind = TokenKind::Identifier;
    }
    formToken(token, end, kind);
    return;
  }

  if (charinfo::isDigit(*BufferPtr)) {
    const char* end = BufferPtr + 1;
    while (charinfo::isDigit(*end))
      end++;
    formToken(token, end, TokenKind::Number);
    return;
  }

  if (*BufferPtr == '"') {
    const char* end = BufferPtr + 1;
    while (*end && *end != '"')
      ++end;
    if (*end == '"') {
      ++BufferPtr;
      formToken(token, end, String);
      BufferPtr = end + 1;
    } else {
      formToken(token, BufferPtr + 1, Unknown);
    }
    return;
  }

  if (*BufferPtr == '=' && *(BufferPtr + 1) == '=') {
    formToken(token, BufferPtr + 2, Equal);
  } else if (*BufferPtr == '<' && *(BufferPtr + 1) == '=') {
    formToken(token, BufferPtr + 2, LessEq);
  } else if (*BufferPtr == '>' && *(BufferPtr + 1) == '=') {
    formToken(token, BufferPtr + 2, GreaterEq);
  } else if (*BufferPtr == '!' && *(BufferPtr + 1) == '=') {
    formToken(token, BufferPtr + 2, NotEqual);
  } else if (*BufferPtr == '|' && *(BufferPtr + 1) == '|') {
    formToken(token, BufferPtr + 2, Or);
  } else if (*BufferPtr == '&' && *(BufferPtr + 1) == '&') {
    formToken(token, BufferPtr + 2, And);
  } else {
    switch (*BufferPtr) {
#define CASE(ch, tok) \
case ch: formToken(token, BufferPtr + 1, tok); break
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
      default:formToken(token, BufferPtr + 1, Unknown);
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
