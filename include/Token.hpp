#ifndef REPL_TOKEN_H
#define REPL_TOKEN_H

#include <cstdint>
#include <string_view>

/**
 * @brief Kinds of lexical tokens produced by the lexer.
 */
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

/**
 * @brief Source location information for a token.
 */
struct Location
{
    size_t Col;
    size_t Line;
};

/**
 * @brief Representation of a lexical token.
 */
struct Token
{
    TokenKind Kind;
    Location Loc;
    std::string_view Value;
};

#endif // REPL_TOKEN_H
