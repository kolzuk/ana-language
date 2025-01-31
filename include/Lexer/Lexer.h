#ifndef LEXER_H
#define LEXER_H

#include <cstdint>
#include <deque>
#include <map>
#include <string>

#include "Token.h"

class Lexer {
  const char* BufferStart;
  const char* BufferPtr;
  size_t CurrentLine;
  size_t CurrentColumn;

 public:
  explicit Lexer(const std::string& Buffer) {
    BufferStart = Buffer.c_str();
    BufferPtr = BufferStart;
    CurrentLine = 1;
    CurrentColumn = 1;
  }

  void next(Token& Token);
  Token peek(int n);

 private:
  void formToken(Token& Result, const char* TokEnd, TokenKind Kind);
  std::deque<Token> TokensBuffer;
};

#endif //LEXER_H