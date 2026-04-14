#include "Lexer.hpp"
#include <array>
#include <format>
#include <iostream>
#include <assert.h>


namespace
{
	constexpr bool TrimWhitespaceEnabled = true;

    /// <summary>
    /// Convert a char to unsigned char for safe use with character classification APIs.
    /// </summary>
    /// <param name="c">The character to convert.</param>
    /// <returns>The converted unsigned char value.</returns>
    constexpr unsigned char ToUChar(char c) noexcept
    {
        return static_cast<unsigned char>(c);
    }

    /// <summary>
    /// Build the fixed lookup table for single-character literal token kinds.
    /// </summary>
    /// <returns>The initialized character token lookup table.</returns>
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
        literals[ToUChar('(')]  = TokenKind::OpenParen;
        literals[ToUChar(')')]  = TokenKind::CloseParen;
        literals[ToUChar('{')]  = TokenKind::OpenCurly;
        literals[ToUChar('}')]  = TokenKind::CloseCurly;

        return literals;
    }

    constexpr auto CharLiterals = MakeCharLiterals();

    /// <summary>
    /// Convert a TokenKind enum value into a human-readable text label.
    /// </summary>
    /// <param name="aKind">The token kind to convert.</param>
    /// <returns>The token kind text label.</returns>
    std::string_view TokenKindToText(TokenKind aKind)
    {
        switch (aKind)
        {
		using enum TokenKind;
        case End:            return "End";
        case Invalid:        return "Invalid";
        case Whitespace:     return "Whitespace";
        case NewLine:        return "NewLine";
        case Tab:            return "Tab";
        case Keyword:        return "Keyword";
        case Identifier:     return "Identifier";
        case IntLiteral:     return "IntLiteral";
        case StringLiteral:  return "StringLiteral";
        case Preprocessor:   return "Preprocessor";
        case Semicolon:      return "Semicolon";
        case Colon:          return "Colon";
        case Comma:          return "Comma";
        case OpenParen:      return "OpenParen";
        case CloseParen:     return "CloseParen";
        case OpenCurly:      return "OpenCurly";
        case CloseCurly:     return "CloseCurly";
		}

		assert(false && "Invalid TokenKind");
		return "";
    }

    /// <summary>
    /// Format a token value for display, escaping whitespace characters.
    /// </summary>
    /// <param name="arView">The token value to format.</param>
    /// <returns>The escaped display string view.</returns>
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

    /// <summary>
    /// Serialize a Token to a text representation for debugging output.
    /// </summary>
    /// <param name="aToken">The token to serialize.</param>
    /// <returns>The formatted token string.</returns>
    std::string TokenToText(const Token& aToken)
    {
        return std::format("{} : {} : {{{}, {}}}", TokenKindToText(aToken.Kind), 
			TokenValueToText(aToken.Value), aToken.Loc.Line, aToken.Loc.Col);
    }

    /// <summary>
    /// Determine whether a character may start an identifier.
    /// </summary>
    /// <param name="c">The character to test.</param>
    /// <returns>True if the character may start an identifier.</returns>
	bool StartsIdentifier(unsigned char c)
	{
		return isalpha(c) || c == '_';
	}

    /// <summary>
    /// Determine whether a character may continue an identifier.
    /// </summary>
    /// <param name="c">The character to test.</param>
    /// <returns>True if the character may continue an identifier.</returns>
	bool ContinuesIdentifier(unsigned char c)
	{
		return isalnum(c) || c == '_';
	}
}

/// <summary>
/// Print the lexer input content to stdout.
/// </summary>
void Lexer::PrintContent()
{
    std::cout << Content << '\n';
}

/// <summary>
/// Print all generated tokens to stdout.
/// </summary>
void Lexer::PrintTokens()
{
    for (const auto& token : Tokens)
    {
        std::cout << TokenToText(token) << '\n';
    }
}

/// <summary>
/// Consume whitespace characters and update line/column tracking.
/// </summary>
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

/// <summary>
/// Tokenize the entire input buffer into the token list.
/// </summary>
void Lexer::Tokenize()
{
    while (QCursorValid())
    {
        ConsumeToken();
    }
}

/// <summary>
/// Consume the next token from the input and add it to the token list.
/// </summary>
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
        const size_t begin = Cursor;
        while (QCursorValid() && PeekCursor() != '\n')
        {
            IterateChar();
        }
        token.Value = Content.substr(begin, Cursor - begin);
        token.Loc = { LineIdx, begin - StartOfLine };
    }
    else
    {
        token.Value = Content.substr(Cursor, 1);
        token.Loc = { LineIdx, Cursor - StartOfLine };
        IterateChar();
    }

    PushToken(std::move(token));
}

/// <summary>
/// Advance the read cursor by one character.
/// </summary>
inline void Lexer::IterateChar()
{
	++Cursor;
}

/// <summary>
/// Advance the read cursor by the specified number of characters.
/// </summary>
/// <param name="aCount">The number of characters to advance.</param>
inline void Lexer::IterateChars(size_t aCount)
{
	Cursor += aCount;
}
