#ifndef REPL_TOKEN_H
#define REPL_TOKEN_H

#include <cstdint>
#include <string_view>
#include <array>

constexpr size_t MaxCompoundTokenOptions = 4;

/**
 * @brief Kinds of lexical tokens produced by the lexer.
 */
enum class TokenKind : uint8_t
{
    // Special Cases
    Invalid = 0, End,

    // Whitespaces
    Whitespace, NewLine, Tab,

    // Comments
    LineComment, BlockComment,

    // Keywords and Identifiers
    Keyword, Identifier,

    // Literals
    IntLiteral, FloatLiteral, HalfFloatLiteral, StringLiteral, CharLiteral,

    // Preprocessor
    Preprocessor,

    // Multi-Character Literals
    /// Compound Operators
    PlusEqual, MinusEqual, Increment, Decrement, AsteriskEqual, SlashEqual, PercentEqual,
    EqualsEquals, NotEquals, LogicalAnd, LogicalOr,
    LessEqual, GreaterEqual,

    // Single Character Literals
    /// Operators
    Plus, Minus, Asterisk, Slash, Percent,
    Ampersand, Pipe, Caret, Tilde, Bang, Equal, Question,
    /// Delimiters
    Semicolon, Colon, Comma, Period,
    /// Scopes and Grouping
    OpenParen, CloseParen, OpenCurly, CloseCurly,
    OpenBracket, CloseBracket, OpenAngle, CloseAngle,
};

/**
 * @brief Source location information for a token.
 */
struct Location
{
    size_t Line;
    size_t Col;
};

struct TokenLiteral
{
    std::string_view Text;
    TokenKind Kind;
};

struct TokenLiteralInfo
{
    TokenKind Single;
    std::array<TokenLiteral, MaxCompoundTokenOptions> CompoundOptions;
    size_t CompoundCount;
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
