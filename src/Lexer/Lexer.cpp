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

void Lexer::next(Token &token) {
  while (*BufferPtr &&
          charinfo::isWhitespace(*BufferPtr))
    ++BufferPtr;

  if (!*BufferPtr) {
    token.Kind = TokenKind::EOI;
    return;
  }

  if (charinfo::isLetter(*BufferPtr)) {
    const char *end = BufferPtr + 1;
    while (charinfo::isLetter(*end))
      ++end;

    llvm::StringRef Name(BufferPtr, end - BufferPtr);

    TokenKind kind;
    if (Name == "integer") {
      kind = TokenKind::IntegerKW;
    } else {
      kind = TokenKind::Identifier;
    }

    formToken(token, end, kind);
    return;
  } else if (charinfo::isDigit(*BufferPtr)) {
    const char *end = BufferPtr + 1;
    while (charinfo::isDigit(*end))
      end++;
    formToken(token, end, TokenKind::Number);
    return;
  } else {
    switch (*BufferPtr) {
#define CASE(ch, tok) \
    case ch: formToken(token, BufferPtr + 1, tok); break
      CASE('+', TokenKind::Plus);
      CASE('-', TokenKind::Minus);
      CASE('*', TokenKind::Star);
      CASE('/', TokenKind::Slash);
      CASE('(', TokenKind::LParen);
      CASE(')', TokenKind::RParen);
      CASE(':', TokenKind::Colon);
      CASE(',', TokenKind::Comma);
      CASE(';', TokenKind::Semicolon);
      CASE('=', TokenKind::Assign);
#undef CASE
      default:
        formToken(token, BufferPtr + 1, TokenKind::Unknown);
    }
    return;
  }
}

void Lexer::formToken(Token &Result,
                      const char *TokEnd,
                      TokenKind Kind) {
  Result.Kind = Kind;
  Result.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
  BufferPtr = TokEnd;
}
