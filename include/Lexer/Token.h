#ifndef TOKEN_H
#define TOKEN_H

enum class TokenKind : short {
  Identifier,
  Number,
  Colon,     // ":'
  Semicolon, // ";"
  Comma,     // ","

  Plus, // "+"
  Minus, // "-"
  Star, // "*"
  Slash, // "/"
  Percent, // "%"
  Assign, // "="

  Equal, // "=="
  NotEqual, // "!="
  Less, // "<"
  LessEq, // "<="
  Greater, // ">"
  GreaterEq, // ">="

  Or, // "||"
  And, // "&&"

  LParen, // "("
  RParen, // ")"
  LFigure, // "{"
  RFigure, // "}"
  LSquare, // "["
  RSquare, // "]"
  Arrow, // "->"

  // Keywords
  KW_integer,
  KW_array,
  KW_if,
  KW_else,
  KW_for,
  KW_while,
  KW_return,
  KW_fun,
  KW_print,
  KW_break,
  KW_continue,
  KW_new,

  EOI, // End of input
  Unknown,
};

class Token {
  friend class Lexer;

 private:
  TokenKind Kind;
  std::string Text;
  size_t Line;
  size_t Column;

 public:
  [[nodiscard]] TokenKind getKind() const {
    return Kind;
  }

  [[nodiscard]] std::string getText() const {
    return Text;
  }

  [[nodiscard]] size_t getLine() const {
    return Line;
  }

  [[nodiscard]] size_t getColumn() const {
    return Column;
  }

  [[nodiscard]] bool is(TokenKind K) const {
    return Kind == K;
  }

  [[nodiscard]] bool isOneOf(TokenKind K1, TokenKind K2) const {
    return is(K1) || is(K2);
  }

  template<typename... Ts>
  bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks) const {
    return is(K1) || isOneOf(K2, Ks...);
  }

  [[nodiscard]] static std::string toString(TokenKind K) {
    switch (K) {
      case TokenKind::Identifier: return "Identifier";
      case TokenKind::Number: return "Number";
      case TokenKind::Colon: return ":";
      case TokenKind::Semicolon: return ";";
      case TokenKind::Comma: return ",";
      case TokenKind::Plus: return "+";
      case TokenKind::Minus: return "-";
      case TokenKind::Star: return "*";
      case TokenKind::Slash: return "/";
      case TokenKind::Percent: return "%";
      case TokenKind::Assign: return "=";
      case TokenKind::Equal: return "==";
      case TokenKind::NotEqual: return "!=";
      case TokenKind::Less: return "<";
      case TokenKind::LessEq: return "<=";
      case TokenKind::Greater: return ">";
      case TokenKind::GreaterEq: return ">=";
      case TokenKind::Or: return "||";
      case TokenKind::And: return "&&";
      case TokenKind::LParen: return "(";
      case TokenKind::RParen: return ")";
      case TokenKind::LFigure: return "{";
      case TokenKind::RFigure: return "}";
      case TokenKind::LSquare: return "]";
      case TokenKind::RSquare: return "[";
      case TokenKind::Arrow: return "0>";
      case TokenKind::KW_integer: return "integer";
      case TokenKind::KW_array: return "array";
      case TokenKind::KW_if: return "if";
      case TokenKind::KW_else: return "else";
      case TokenKind::KW_for: return "for";
      case TokenKind::KW_while: return "while";
      case TokenKind::KW_return: return "return";
      case TokenKind::KW_fun: return "fun";
      case TokenKind::KW_print: return "print";
      case TokenKind::KW_break: return "break";
      case TokenKind::KW_continue: return "continue";
      case TokenKind::KW_new: return "new";
      case TokenKind::EOI: return "EOI";
      case TokenKind::Unknown: return "Unknown";

    }
  }
};

#endif //TOKEN_H