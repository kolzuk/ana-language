#include "Lexer/Lexer.h"

namespace charinfo {
inline bool isLinebreak(char c) {
  return c == '\v' || c == '\r' || c == '\n';
}
inline bool isWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\f' ||
      isLinebreak(c);
}
inline bool isDigit(char c) {
  return c >= '0' && c <= '9';
}
inline bool isLetter(char c) {
  return (c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z');
}
}

void Lexer::next(Token& Token) {
  if (!TokensBuffer.empty()) {
    Token = TokensBuffer[0];
    TokensBuffer.pop_front();
    return;
  }

  while (*BufferPtr && charinfo::isWhitespace(*BufferPtr)) {
    ++CurrentColumn;
    if (charinfo::isLinebreak(*BufferPtr))
    {
      CurrentColumn = 0;
      ++CurrentLine;
    }
    ++BufferPtr;
  }

  while (*BufferPtr == '#') { // skip comments
    while (*BufferPtr && !charinfo::isLinebreak(*BufferPtr)) {
      ++BufferPtr;
      ++CurrentColumn;
    }
    CurrentColumn = 0;
    ++CurrentLine;
    while (*BufferPtr && charinfo::isWhitespace(*BufferPtr)) {
      ++CurrentColumn;
      if (charinfo::isLinebreak(*BufferPtr))
      {
        CurrentColumn = 0;
        ++CurrentLine;
      }
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
    std::string Name(BufferPtr, End - BufferPtr);
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
    } else if (Name == "array") {
      Kind = TokenKind::KW_array;
    } else if (Name == "fun") {
      Kind = TokenKind::KW_fun;
    } else if (Name == "print") {
      Kind = TokenKind::KW_print;
    } else if (Name == "break") {
      Kind = TokenKind::KW_break;
    } else if (Name == "continue") {
      Kind = TokenKind::KW_continue;
    } else if (Name == "new") {
      Kind = TokenKind::KW_new;
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
  } else if (*BufferPtr == '-' && *(BufferPtr + 1) == '>') {
    formToken(Token, BufferPtr + 2, TokenKind::Arrow);
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

Token Lexer::peek(int n) {
  for (auto i = TokensBuffer.size(); i <= n; i++) {
    Token Token;
    next(Token);
    TokensBuffer.push_back(Token);
  }
  return TokensBuffer[n];
}

void Lexer::formToken(Token& Result,
                      const char* TokEnd,
                      TokenKind Kind) {
  Result.Kind = Kind;
  Result.Text = std::string(BufferPtr, TokEnd - BufferPtr);
  CurrentColumn += TokEnd - BufferPtr;
  BufferPtr = TokEnd;
  Result.Line = CurrentLine;
  Result.Column = CurrentColumn;
}