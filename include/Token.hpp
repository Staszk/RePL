#ifndef REPL_TOKEN_H
#define REPL_TOKEN_H

#include <cstdint>
#include <string_view>

enum class TokenKind : uint8_t
{
    // Special Cases
    Invalid = 0, End,
    // Whitespaces
    Whitespace, NewLine, Tab,
    // Keywords and Identifiers
    Keyword, Identifier,
    // Literals
    IntLiteral, StringLiteral,
    // Preprocessor
    Preprocessor,
    // Single Character Literals
    Semicolon, Colon, Comma, OpenParen, CloseParen, OpenCurly, CloseCurly,
};

struct Location
{
    size_t Col;
    size_t Line;
};

struct Token
{
    TokenKind Kind;
    Location Loc;
    std::string_view Value;
};

#endif // REPL_TOKEN_H
