#include "Lexer.hpp"
#include <array>
#include <format>
#include <iostream>
#include <assert.h>

namespace
{
    constexpr bool TrimWhitespaceEnabled = true;

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

    /**
     * @brief Build the fixed lookup table for single-character literal token kinds.
     *
     * @return The initialized character token lookup table.
     */
    constexpr std::array<TokenKind, 256> MakeCharLiterals()
    {
        std::array<TokenKind, 256> literals;
        literals.fill(TokenKind::Invalid);

        literals[ToUChar('\0')] = TokenKind::End;
        literals[ToUChar(' ')]  = TokenKind::Whitespace;
        literals[ToUChar('\n')] = TokenKind::NewLine;
        literals[ToUChar('\t')] = TokenKind::Tab;
        literals[ToUChar(';')]  = TokenKind::Semicolon;
        literals[ToUChar(':')]  = TokenKind::Colon;
        literals[ToUChar(',')]  = TokenKind::Comma;
        literals[ToUChar('.')]  = TokenKind::Period;
        literals[ToUChar('(')]  = TokenKind::OpenParen;
        literals[ToUChar(')')]  = TokenKind::CloseParen;
        literals[ToUChar('{')]  = TokenKind::OpenCurly;
        literals[ToUChar('}')]  = TokenKind::CloseCurly;
        literals[ToUChar('[')]  = TokenKind::OpenBracket;
        literals[ToUChar(']')]  = TokenKind::CloseBracket;
        literals[ToUChar('<')]  = TokenKind::OpenAngle;
        literals[ToUChar('>')]  = TokenKind::CloseAngle;

        return literals;
    }

    constexpr auto CharLiterals = MakeCharLiterals();

    /**
     * @brief Convert a TokenKind enum value into a human-readable text label.
     *
     * @param aKind The token kind to convert.
     * @return The token kind text label.
     */
    constexpr std::string_view TokenKindToText(TokenKind aKind)
    {
        switch (aKind)
        {
        using enum TokenKind;
        case Invalid:        return "Invalid";
        case End:            return "End";
        case Whitespace:     return "Whitespace";
        case NewLine:        return "NewLine";
        case Tab:            return "Tab";
        case LineComment:    return "LineComment";
        case BlockComment:   return "BlockComment";
        case Keyword:        return "Keyword";
        case Identifier:     return "Identifier";
        case IntLiteral:     return "IntLiteral";
        case FloatLiteral:   return "FloatLiteral";
        case StringLiteral:  return "StringLiteral";
        case CharLiteral:    return "CharLiteral";
        case Preprocessor:   return "Preprocessor";
        case Semicolon:      return "Semicolon";
        case Colon:          return "Colon";
        case Comma:          return "Comma";
        case Period:         return "Period";
        case OpenParen:      return "OpenParen";
        case CloseParen:     return "CloseParen";
        case OpenCurly:      return "OpenCurly";
        case CloseCurly:     return "CloseCurly";
        case OpenBracket:    return "OpenBracket";
        case CloseBracket:   return "CloseBracket";
        case OpenAngle:      return "OpenAngle";
        case CloseAngle:     return "CloseAngle";
        }

        assert(false && "Invalid TokenKind");
        return "";
    }

    /**
     * @brief Format a token value for display, escaping whitespace characters.
     *
     * @param arView The token value to format.
     * @return The escaped display string view.
     */
    std::string_view TokenValueToText(const std::string_view arView)
    {
        std::string_view v{ arView };

        if (arView.size() == 1)
        {
            if (arView.at(0) == '\n')
            {
                v = "\\n";
            }
            else if (arView.at(0) == '\t')
            {
                v = "\\t";
            }
            else if (arView.at(0) == ' ')
            {
                v = "' '";
            }
        }

        return v;
    }

    /**
     * @brief Serialize a Token to a text representation for debugging output.
     *
     * @param aToken The token to serialize.
     * @return The formatted token string.
     */
    std::string TokenToText(const Token& aToken)
    {
        return std::format("{} : {} : {{{}, {}}}", TokenKindToText(aToken.Kind),
            TokenValueToText(aToken.Value), aToken.Loc.Line + 1, aToken.Loc.Col + 1);
    }

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
 */
Lexer::Lexer(const char *apText)
    : Content(apText), Cursor(0), LineIdx(0), StartOfLine(0)
{
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
        std::cout << TokenToText(token) << '\n';
    }
}

/**
 * @brief Consume whitespace characters and update line/column tracking.
 */
void Lexer::TrimWhitespace()
{
    while (QCursorValid() && std::isspace(ToUChar(Content[Cursor])))
    {
        if (Content[Cursor] == '\n')
        {
            ++LineIdx;
            StartOfLine = Cursor + 1;
        }

        IterateChar();
    }
}

/**
 * @brief Tokenize the entire input buffer into the token list.
 */
void Lexer::Tokenize()
{
    while (QCursorValid())
    {
        ConsumeToken();
    }
}

/**
 * @brief Consume the next token from the input and add it to the token list.
 */
void Lexer::ConsumeToken()
{
    if constexpr (TrimWhitespaceEnabled)
    {
        TrimWhitespace();
        if (!QCursorValid())
        {
            return;
        }
    }

    Token token{};

    const char c = Content[Cursor];
    const TokenKind kind = CharLiterals[ToUChar(c)];

    if (kind != TokenKind::Invalid)
    {
        token.Kind = kind;
        token.Value = Content.substr(Cursor, 1);
        token.Loc = { LineIdx, Cursor - StartOfLine };

        if constexpr (!TrimWhitespaceEnabled)
        {
            if (token.Kind == TokenKind::NewLine)
            {
                ++LineIdx;
                StartOfLine = Cursor + 1;
            }
        }

        IterateChar();
    }
    else if (std::isdigit(ToUChar(c)))
    {
        token.Kind = TokenKind::IntLiteral;
        const size_t begin = Cursor;
        IterateChar(); // Consume first digit
        while (QCursorValid() && std::isdigit(ToUChar(PeekCursor())))
        {
            IterateChar();
        }
        token.Value = Content.substr(begin, Cursor - begin);
        token.Loc = { LineIdx, begin - StartOfLine };
    }
    else if (StartsIdentifier(ToUChar(c)))
    {
        token.Kind = TokenKind::Identifier;
        const size_t begin = Cursor;
        IterateChar(); // Consume first character
        while (QCursorValid() && ContinuesIdentifier(ToUChar(PeekCursor())))
        {
            IterateChar();
        }
        token.Value = Content.substr(begin, Cursor - begin);
        token.Loc = { LineIdx, begin - StartOfLine };
    }
    else if (c == '#')
    {
        token.Kind = TokenKind::Preprocessor;
        const size_t lineIdxBegin = LineIdx;
        const size_t lineBegin = StartOfLine;
        const size_t tokenBegin = Cursor;
        while (QCursorValid() && PeekCursor() != '\n')
        {
            if (PeekCursor() == '\\') // Handle line continuation
            {
                IterateChar(); // Consume backslash
                if (QCursorValid() && PeekCursor() == '\n')
                {
                    ++LineIdx;
                    StartOfLine = Cursor + 1;
                    IterateChar(); // Consume newline
                }
            }
            else
            {
                IterateChar();
            }
        }
        token.Value = Content.substr(tokenBegin, Cursor - tokenBegin);
        token.Loc = { lineIdxBegin, tokenBegin - lineBegin };
    }
    else if (c == '"')
    {
        token.Kind = TokenKind::StringLiteral;
        const size_t begin = Cursor;
        IterateChar(); // Consume opening quote
        while (QCursorValid() && PeekCursor() != '"')
        {
            if (PeekCursor() == '\\') // Handle escape sequences
            {
                IterateChar(); // Consume backslash
                if (QCursorValid())
                {
                    IterateChar(); // Consume escaped character
                }
            }
            else
            {
                IterateChar();
            }
        }
        if (QCursorValid())
        {
            IterateChar(); // Consume closing quote
        }
        token.Value = Content.substr(begin, Cursor - begin);
        token.Loc = { LineIdx, begin - StartOfLine };
    }
    else if (c == '\'')
    {
        token.Kind = TokenKind::CharLiteral;
        const size_t begin = Cursor;
        IterateChar(); // Consume opening quote
        if (QCursorValid())
        {
            if (PeekCursor() == '\\') // Handle escape sequences
            {
                IterateChar(); // Consume backslash
                if (QCursorValid())
                {
                    IterateChar(); // Consume escaped character
                }
            }
            else
            {
                IterateChar(); // Consume character
            }
        }

        if (QCursorValid() && PeekCursor() == '\'')
        {
            IterateChar(); // Consume closing quote
        }
        token.Value = Content.substr(begin, Cursor - begin);
        token.Loc = { LineIdx, begin - StartOfLine };
    }
    else if (QCursorValid() && StartsLineComment(ToUChar(c), ToUChar(PeekAt(1))))
    {
        token.Kind = TokenKind::LineComment;
        const size_t begin = Cursor;
        while (QCursorValid() && PeekCursor() != '\n')
        {
            IterateChar();
        }
        token.Value = Content.substr(begin, Cursor - begin);
        token.Loc = { LineIdx, begin - StartOfLine };
    }
    else if (QCursorValid() && StartsBlockComment(ToUChar(c), ToUChar(PeekAt(1))))
    {
        token.Kind = TokenKind::BlockComment;
        const size_t lineIdxBegin = LineIdx;
        const size_t lineBegin = StartOfLine;
        const size_t tokenBegin = Cursor;
        IterateChars(2); // Consume opening '/*'
        while (QCursorValid() && !EndsBlockComment(ToUChar(PeekCursor()), ToUChar(PeekAt(1))))
        {
            if (PeekCursor() == '\n')
            {
                ++LineIdx;
                StartOfLine = Cursor + 1;
            }
            IterateChar();
        }
        IterateChars(2); // Consume closing '*/'
        token.Value = Content.substr(tokenBegin, Cursor - tokenBegin);
        token.Loc = { lineIdxBegin, tokenBegin - lineBegin };
    }
    else
    {
        token.Value = Content.substr(Cursor, 1);
        token.Loc = { LineIdx, Cursor - StartOfLine };
        IterateChar();
    }

    PushToken(std::move(token));
}

/**
 * @brief Advance the read cursor by one character.
 */
inline void Lexer::IterateChar()
{
    ++Cursor;
}

/**
 * @brief Advance the read cursor by the specified number of characters.
 *
 * @param aCount The number of characters to advance.
 */
inline void Lexer::IterateChars(size_t aCount)
{
    Cursor += aCount;
}

/**
 * @brief Peek at a character at a specific offset from the current cursor position.
 *
 * @param offset The offset from the current cursor position.
 * @return A reference to the character at the specified offset.
 */
const char &Lexer::PeekAt(size_t offset) const
{
    if (Cursor + offset < Content.size())
    {
        return Content.at(Cursor + offset);
    }
    return Content.back();
}
