#ifndef REPL_TOKEN_HPP
#define REPL_TOKEN_HPP

#include <cstdint>
#include <string>
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

    // Identifiers
    Identifier,

    // Literals
    KeywordLiteral, IntLiteral, FloatLiteral, HalfFloatLiteral, StringLiteral, CharLiteral,

    // Preprocessor
    Preprocessor,

    // Multi-Character Literals
    /// Compound Operators
    PlusEqual, MinusEqual, Increment, Decrement, AsteriskEqual, SlashEqual, PercentEqual,
    EqualsEquals, NotEquals, LogicalAnd, LogicalOr,
    LessEqual, GreaterEqual,
    Arrow,

    // Single Character Literals
    /// Operators
    Plus, Minus, Asterisk, Slash, Percent,
    Ampersand, Pipe, Caret, Tilde, Bang, Equal, Question,
    Less, Greater,
    /// Delimiters
    Semicolon, Colon, Comma, Period,
    /// Scopes and Grouping
    OpenParen, CloseParen, OpenCurly, CloseCurly,
    OpenBracket, CloseBracket,
};

/**
 * @brief Source location information for a token.
 */
struct TokenLocation
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
    TokenLocation Loc;
    std::string_view Value;
};

namespace TokenHelpers
{
    std::string LocationToText(const TokenLocation& loc);
    constexpr std::string_view TokenKindToText(TokenKind aKind);
    std::string_view TokenValueToText(const std::string_view arView);
    std::string TokenToText(const Token& aToken);
}
#endif // REPL_TOKEN_H
