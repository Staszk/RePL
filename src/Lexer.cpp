#include "Lexer.hpp"
#include <algorithm>
#include <format>
#include <iostream>
#include <assert.h>

namespace
{
    /**
    * @brief Convert a WarningLevel enum value into a human-readable text label.
    *
    * @param aLevel The warning level to convert.
    * @return The warning level text label.
    */
    constexpr std::string_view WarningLevelToText(WarningLevel aLevel)
    {
        switch (aLevel)
        {
        using enum WarningLevel;
        case Info:    return "Info";
        case Warning: return "Warning";
        case Error:   return "Error";
        }

        assert(false && "Invalid WarningLevel");
        return "";
    }

    /**
     * @brief Convert a char to unsigned char for safe use with character classification APIs.
     *
     * @param c The character to convert.
     * @return The converted unsigned char value.
     */
    constexpr unsigned char ToUChar(char c) noexcept
    {
        return static_cast<unsigned char>(c);
    }

    constexpr std::array<std::string_view, 11> KeywordLiterals =
    {
        "break",
        "continue",
        "else",
        "for",
        "if",
        "struct",
        "while",
        "until"
        "void",
        "true",
        "false",
    };

    /**
     * @brief Check if a string value is a reserved keyword.
     * 
     * @param value The string value to check.
     * @return True if the value is a keyword, false otherwise.
     */
    constexpr bool IsKeywordLiteral(std::string_view value)
    {
        return std::find(KeywordLiterals.begin(), KeywordLiterals.end(), value) != KeywordLiterals.end();
    }

    constexpr std::array<TokenLiteralInfo, 256> MakeTokenLiteralInfo()
    {
        std::array<TokenLiteralInfo, 256> info;
        info.fill({ TokenKind::Invalid, {}, 0 });

        info[ToUChar('\0')] = {TokenKind::End, {}, 0};
        info[ToUChar(' ')]  = {TokenKind::Whitespace, {}, 0};
        info[ToUChar('\n')] = {TokenKind::NewLine, {}, 0};
        info[ToUChar('\t')] = {TokenKind::Tab, {}, 0};
        info[ToUChar(';')]  = {TokenKind::Semicolon, {}, 0};
        info[ToUChar(':')]  = {TokenKind::Colon, {}, 0};
        info[ToUChar(',')]  = {TokenKind::Comma, {}, 0};
        info[ToUChar('.')]  = {TokenKind::Period, {}, 0};
        info[ToUChar('(')]  = {TokenKind::OpenParen, {}, 0};
        info[ToUChar(')')]  = {TokenKind::CloseParen, {}, 0};
        info[ToUChar('{')]  = {TokenKind::OpenCurly, {}, 0};
        info[ToUChar('}')]  = {TokenKind::CloseCurly, {}, 0};
        info[ToUChar('[')]  = {TokenKind::OpenBracket, {}, 0};
        info[ToUChar(']')]  = {TokenKind::CloseBracket, {}, 0};
        info[ToUChar('^')]  = {TokenKind::Caret, {}, 0};
        info[ToUChar('~')]  = {TokenKind::Tilde, {}, 0};
        info[ToUChar('?')]  = {TokenKind::Question, {}, 0};
        
        info[ToUChar('+')]  = { TokenKind::Plus, {{ {"++", TokenKind::Increment}, {"+=", TokenKind::PlusEqual} }}, 2 };
        info[ToUChar('-')]  = { TokenKind::Minus, {{ {"--", TokenKind::Decrement}, {"-=", TokenKind::MinusEqual}, {"->", TokenKind::Arrow} }}, 2 };
        info[ToUChar('*')]  = { TokenKind::Asterisk, {{ {"*=" , TokenKind::AsteriskEqual} }}, 1 };
        info[ToUChar('/')]  = { TokenKind::Slash, {{ {"/=" , TokenKind::SlashEqual} }}, 1 };
        info[ToUChar('%')]  = { TokenKind::Percent, {{ {"%=" , TokenKind::PercentEqual} }}, 1 };
        info[ToUChar('&')]  = { TokenKind::Ampersand, {{ {"&&", TokenKind::LogicalAnd} }}, 1 };
        info[ToUChar('|')]  = { TokenKind::Pipe, {{ {"||", TokenKind::LogicalOr} }}, 1 };
        info[ToUChar('=')]  = { TokenKind::Equal, {{ {"==", TokenKind::EqualEqual} }}, 1 };
        info[ToUChar('!')]  = { TokenKind::Bang, {{ {"!=" , TokenKind::NotEqual} }}, 1 };
        info[ToUChar('<')]  = { TokenKind::Lesser, {{ {"<=" , TokenKind::LesserEqual} }}, 1 };
        info[ToUChar('>')]  = { TokenKind::Greater, {{ {">=" , TokenKind::GreaterEqual} }}, 1 };

        return info;
    }

    constexpr auto TokenLiteralInfoTable = MakeTokenLiteralInfo();

    /**
     * @brief Determine whether a character may start an identifier.
     *
     * @param c The character to test.
     * @return True if the character may start an identifier.
     */
    bool StartsIdentifier(unsigned char c)
    {
        return isalpha(c) || c == '_';
    }

    /**
     * @brief Determine whether a character may continue an identifier.
     *
     * @param c The character to test.
     * @return True if the character may continue an identifier.
     */
    bool ContinuesIdentifier(unsigned char c)
    {
        return isalnum(c) || c == '_';
    }


    /**
     * @brief Check if a pair of characters starts a line comment.
     * 
     * @param c1 The first character.
     * @param c2 The second character.
     * @return True if the characters start a line comment.
     */
    bool StartsLineComment(unsigned char c1, unsigned char c2)
    {
        return c1 == '/' && c2 == '/';
    }

    /**
     * @brief Check if a pair of characters starts a block comment.
     * 
     * @param c1 The first character.
     * @param c2 The second character.
     * @return True if the characters start a block comment.
     */
    bool StartsBlockComment(unsigned char c1, unsigned char c2)
    {
        return c1 == '/' && c2 == '*';
    }

    /**
     * @brief Check if a pair of characters ends a block comment.
     * 
     * @param c1 The first character.
     * @param c2 The second character.
     * @return True if the characters end a block comment.
     */
    bool EndsBlockComment(unsigned char c1, unsigned char c2)
    {
        return c1 == '*' && c2 == '/';
    }
}

/**
 * @brief Construct a new Lexer instance.
 *
 * @param apText The input text to tokenize.
 * @param abEnableMetrics Whether to print performance metrics.
 */
Lexer::Lexer(std::string_view apText, const bool abEnableMetrics /* = true */)
    : Content(apText), ContentSize(Content.size()), StartTime(std::chrono::system_clock::now()), EnableMetrics(abEnableMetrics)
{
    if (EnableMetrics)
    {
        std::cout << "| RePL Lexer Begin at " << std::format("{:%H:%M}", StartTime) << '\n';
    }
    Tokenize();
}

/**
 * @brief Print the lexer input content to stdout.
 */
void Lexer::PrintContent()
{
    std::cout << Content << '\n';
}

/**
 * @brief Print all generated tokens to stdout.
 */
void Lexer::PrintTokens()
{
    for (const auto& token : Tokens)
    {
        std::cout << TokenHelpers::TokenToText(token) << '\n';
    }
}

/**
 * @brief Print all generated warnings to stdout.
 */
void Lexer::PrintWarnings()
{
    if (Warnings.empty())
    {
        return;
    }

    for (const auto& warning : Warnings)
    {
        std::cout << std::format("{} : {} : {}", WarningLevelToText(warning.Level), warning.Message, TokenHelpers::LocationToText(warning.Loc)) << '\n';
    }
}

/**
 * @brief Print lexer performance metrics to stdout.
 */
void Lexer::PrintMetrics()
{
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(EndTime - StartTime);
    std::cout << "| RePL Lexer Completed at " << std::format("{:%H:%M}", EndTime) << '\n';
    std::cout << "| Tokenization completed in " << duration.count() << " microseconds.\n";
    std::cout << "| Average time per token: " << (Tokens.empty() ? 0 : static_cast<double>(duration.count()) / Tokens.size()) << " microseconds\n";
    std::cout << "| Total tokens: " << Tokens.size() << '\n';
    std::cout << "| Total warnings: " << Warnings.size() << '\n';
}

/**
 * @brief Tokenize the entire input buffer into the token list.
 */
void Lexer::Tokenize()
{
    while (CursorValid())
    {
        ConsumeToken();
    }

    // Add an EOF token at the end of the token stream
    PushToken({
        .Kind = TokenKind::End, 
        .Loc = { LineIdx, Cursor - StartOfLine },
        .Value = "EOF" 
    });

    EndTime = std::chrono::system_clock::now();
}

/**
 * @brief Consume whitespace characters and update line/column tracking.
 */
void Lexer::TrimWhitespace()
{
    while (CursorValid() && std::isspace(ToUChar(PeekCursor())))
    {
        Advance();
    }
}

/**
 * @brief Consume the next token from the input and add it to the token list.
 */
void Lexer::ConsumeToken()
{
    TrimWhitespace();
    if (!CursorValid())
    {
        return;
    }

    Token token{};

    if (IsLineCommentStart())
    {
        ConsumeLineCommentToken(token);
    }
    else if (IsBlockCommentStart())
    {
        ConsumeBlockCommentToken(token);
    }
    else if (IsPreprocessorDirectiveStart())
    {
        ConsumePreprocessorToken(token);
    }
    else if (IsStringLiteralStart())
    {
        ConsumeStringLiteralToken(token);
    }
    else if (IsCharLiteralStart())
    {
        ConsumeCharLiteralToken(token);
    }
    else if (IsNumericLiteralStart())
    {
        ConsumeNumericLiteralToken(token);
    }
    else if (IsIdentifierOrKeywordStart())
    {
        ConsumeIdentifierOrKeywordToken(token);
    }
    else
    {
        const auto [literalKind, tokenLength] = GatherTokenLiteral();

        if (literalKind != TokenKind::Invalid)
        {
            ConsumeTokenLiteral(token, literalKind, tokenLength);
        }
        else
        {
            ConsumeUnknownToken(token);
        }
    }

    PushToken(std::move(token));
}

/**
 * @brief Check whether the current character represents a token literal and return its
 * kind, also providing the length of the literal if it's a compound token.
 *
 * @return A pair of the token kind and literal length (1 for single-character tokens, >1 for compound tokens).
 */
std::pair<TokenKind, size_t> Lexer::GatherTokenLiteral()
{
    const size_t cursor = Cursor;
    const char current = PeekCursor();
    const TokenLiteralInfo& info = TokenLiteralInfoTable[ToUChar(current)];

    for (size_t index = 0; index < info.CompoundCount; ++index)
    {
        const auto& option = info.CompoundOptions[index];
        const size_t optionLength = option.Text.size();

        if (optionLength == 0 || cursor + optionLength > ContentSize)
        {
            continue;
        }

        if (Content.substr(cursor, optionLength) == option.Text)
        {
            return { option.Kind, optionLength };
        }
    }

    return { info.Single, 1 };
}

/**
 * @brief Check whether the current character starts an integer or float literal.
 *
 * @return True if the current character begins a numeric literal.
 */
bool Lexer::IsNumericLiteralStart()
{
    return std::isdigit(ToUChar(PeekCursor()));
}

/**
 * @brief Check whether the current character starts an identifier.
 *
 * @return True if the current character begins an identifier.
 */
bool Lexer::IsIdentifierOrKeywordStart()
{
    return StartsIdentifier(ToUChar(PeekCursor()));
}

/**
 * @brief Check whether the current character starts a preprocessor directive.
 *
 * @return True if the current character is '#'.
 */
bool Lexer::IsPreprocessorDirectiveStart()
{
    return PeekCursor() == '#';
}

/**
 * @brief Check whether the current character starts a string literal.
 *
 * @return True if the current character is a string quote.
 */
bool Lexer::IsStringLiteralStart()
{
    return PeekCursor() == '"';
}

/**
 * @brief Check whether the current character starts a character literal.
 *
 * @return True if the current character is a character quote.
 */
bool Lexer::IsCharLiteralStart()
{
    return PeekCursor() == '\'';
}

/**
 * @brief Check whether the current characters start a line comment.
 *
 * @return True if the current characters are '//'.
 */
bool Lexer::IsLineCommentStart()
{
    return StartsLineComment(ToUChar(PeekCursor()), ToUChar(PeekAt(1)));
}

/**
 * @brief Check whether the current characters start a block comment.
 *
 * @return True if the current characters are '/*'.
 */
bool Lexer::IsBlockCommentStart()
{
    return StartsBlockComment(ToUChar(PeekCursor()), ToUChar(PeekAt(1)));
}

/**
 * @brief Consume a single-character token.
 *
 * @param arToken The token to populate.
 * @param aKind The kind of token to consume.
 */
void Lexer::ConsumeTokenLiteral(Token& arToken, TokenKind aKind, size_t aLength)
{
    const size_t length = aLength > 0 ? aLength : 1;

    arToken.Kind = aKind;
    arToken.Value = Content.substr(Cursor, length);
    arToken.Loc = { LineIdx, Cursor - StartOfLine };
    AdvanceBy(length);
}

/**
 * @brief Consume an int or float literal token.
 *
 * @param arToken The token to populate.
 */
void Lexer::ConsumeNumericLiteralToken(Token& arToken)
{
    const size_t begin = Cursor;
    Advance(); // Consume first digit
    while (CursorValid() && std::isdigit(ToUChar(PeekCursor())))
    {
        Advance();
    }

    bool isFloat = false;
    if (CursorValid() && PeekCursor() == '.' && std::isdigit(ToUChar(PeekAt(1))))
    {
        isFloat = true;
        Advance(); // Consume decimal point
        while (CursorValid() && std::isdigit(ToUChar(PeekCursor())))
        {
            Advance();
        }
    }

    if (isFloat && CursorValid() && PeekCursor() == 'h')
    {
        arToken.Kind = TokenKind::HalfFloatLiteral;
        Advance(); // Consume the half-float suffix
    }
    else
    {
        arToken.Kind = isFloat ? TokenKind::FloatLiteral : TokenKind::IntLiteral;
    }

    arToken.Value = Content.substr(begin, Cursor - begin);
    arToken.Loc = { LineIdx, begin - StartOfLine };
}

/**
 * @brief Consume an identifier token.
 * 
 * @param arToken The token to populate.
 */
void Lexer::ConsumeIdentifierOrKeywordToken(Token& arToken)
{
    const size_t begin = Cursor;
    Advance(); // Consume first character
    while (CursorValid() && ContinuesIdentifier(ToUChar(PeekCursor())))
    {
        Advance();
    }

    arToken.Value = Content.substr(begin, Cursor - begin);
    arToken.Kind = IsKeywordLiteral(arToken.Value) ? TokenKind::KeywordLiteral : TokenKind::Identifier;
    arToken.Loc = { LineIdx, begin - StartOfLine };
}

/**
 * @brief Consume a preprocessor directive token.
 * 
 * @param arToken The token to populate.
 */
void Lexer::ConsumePreprocessorToken(Token& arToken)
{
    arToken.Kind = TokenKind::Preprocessor;
    const size_t lineIdxBegin = LineIdx;
    const size_t lineBegin = StartOfLine;
    const size_t tokenBegin = Cursor;
    while (CursorValid() && PeekCursor() != '\n')
    {
        if (PeekCursor() == '\\') // Handle line continuation
        {
            Advance(); // Consume backslash
            if (CursorValid() && PeekCursor() == '\n')
            {
                Advance(); // Consume newline and update line tracking
            }
        }
        else
        {
            Advance();
        }
    }
    arToken.Value = Content.substr(tokenBegin, Cursor - tokenBegin);
    arToken.Loc = { lineIdxBegin, tokenBegin - lineBegin };
}

/**
 * @brief Consume a string literal token.
 * 
 * @param arToken The token to populate.
 */
void Lexer::ConsumeStringLiteralToken(Token& arToken)
{
    arToken.Kind = TokenKind::StringLiteral;
    const size_t begin = Cursor;
    Advance(); // Consume opening quote
    while (CursorValid() && PeekCursor() != '"')
    {
        if (PeekCursor() == '\\') // Handle escape sequences
        {
            Advance(); // Consume backslash
            if (CursorValid())
            {
                Advance(); // Consume escaped character
            }
        }
        else
        {
            Advance();
        }
    }
    if (CursorValid())
    {
        Advance(); // Consume closing quote
    }
    arToken.Value = Content.substr(begin, Cursor - begin);
    arToken.Loc = { LineIdx, begin - StartOfLine };
}

/**
 * @brief Consume a character literal token.
 * 
 * @param arToken The token to populate.
 */
void Lexer::ConsumeCharLiteralToken(Token& arToken)
{
    arToken.Kind = TokenKind::CharLiteral;
    const size_t begin = Cursor;
    Advance(); // Consume opening quote
    if (CursorValid())
    {
        if (PeekCursor() == '\\') // Handle escape sequences
        {
            Advance(); // Consume backslash
            if (CursorValid())
            {
                Advance(); // Consume escaped character
            }
        }
        else
        {
            Advance(); // Consume character
        }
    }

    if (CursorValid() && PeekCursor() == '\'')
    {
        Advance(); // Consume closing quote
    }
    arToken.Value = Content.substr(begin, Cursor - begin);
    arToken.Loc = { LineIdx, begin - StartOfLine };
}

/**
 * @brief Consume a line comment token.
 * 
 * @param arToken The token to populate.
 */
void Lexer::ConsumeLineCommentToken(Token& arToken)
{
    arToken.Kind = TokenKind::LineComment;
    const size_t begin = Cursor;
    while (CursorValid() && PeekCursor() != '\n')
    {
        Advance();
    }
    arToken.Value = Content.substr(begin, Cursor - begin);
    arToken.Loc = { LineIdx, begin - StartOfLine };
}

/**
 * @brief Consume a block comment token.
 * 
 * @param arToken The token to populate.
 */
void Lexer::ConsumeBlockCommentToken(Token& arToken)
{
    arToken.Kind = TokenKind::BlockComment;
    const size_t lineIdxBegin = LineIdx;
    const size_t lineBegin = StartOfLine;
    const size_t tokenBegin = Cursor;
    AdvanceBy(2); // Consume opening '/*'
    while (CursorValid() && !EndsBlockComment(ToUChar(PeekCursor()), ToUChar(PeekAt(1))))
    {
        Advance();
    }

    if (CursorValid() && EndsBlockComment(ToUChar(PeekCursor()), ToUChar(PeekAt(1))))
    {
        AdvanceBy(2); // Consume closing '*/'
    }

    arToken.Value = Content.substr(tokenBegin, Cursor - tokenBegin);
    arToken.Loc = { lineIdxBegin, tokenBegin - lineBegin };
}

/**
 * @brief Consume an unknown or unhandled token type.
 *
 * @param arToken The token to populate.
 */
void Lexer::ConsumeUnknownToken(Token& arToken)
{
    arToken.Kind = TokenKind::Invalid;
    arToken.Value = Content.substr(Cursor, 1);
    arToken.Loc = { LineIdx, Cursor - StartOfLine };

    Warnings.push_back({
        .Level = WarningLevel::Error,
        .Message = std::format("Unrecognized token '{}'", arToken.Value),
        .Loc = arToken.Loc
    });

    Advance();
}

/**
 * @brief Advance the read cursor by one character.
 */
inline void Lexer::Advance()
{
    if (!CursorValid())
    {
        return;
    }

    if (PeekCursor() == '\n')
    {
        ++LineIdx;
        StartOfLine = Cursor + 1;
    }

    ++Cursor;
}

/**
 * @brief Advance the read cursor by the specified number of characters.
 *
 * @param aCount The number of characters to advance.
 */
inline void Lexer::AdvanceBy(size_t aCount)
{
    while (aCount-- > 0)
    {
        Advance();
    }
}

/**
 * @brief Peek at a character at a specific offset from the current cursor position.
 *
 * @param offset The offset from the current cursor position.
 * @return The character at the specified offset or '\0' when overrun.
 */
char Lexer::PeekAt(size_t offset) const
{
    const size_t index = Cursor + offset;
    return index < ContentSize ? Content[index] : '\0';
}
