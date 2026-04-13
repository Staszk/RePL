#include "Lexer.hpp"
#include <array>
#include <cctype>
#include <format>
#include <iostream>


namespace
{
	constexpr bool TrimWhitespaceEnabled = true;

    constexpr unsigned char ToUChar(char c) noexcept
    {
        return static_cast<unsigned char>(c);
    }

    constexpr std::array<TokenKind, 256> MakeCharLiterals()
    {
        std::array<TokenKind, 256> literals;
        literals.fill(TokenKind::Invalid);

        literals[ToUChar('\0')] = TokenKind::End;
        literals[ToUChar(' ')] = TokenKind::Whitespace;
        literals[ToUChar('\n')] = TokenKind::NewLine;
        literals[ToUChar('\t')] = TokenKind::Tab;
        literals[ToUChar(';')] = TokenKind::Semicolon;
        literals[ToUChar(':')] = TokenKind::Colon;
        literals[ToUChar(',')] = TokenKind::Comma;
        literals[ToUChar('(')] = TokenKind::OpenParen;
        literals[ToUChar(')')] = TokenKind::CloseParen;
        literals[ToUChar('{')] = TokenKind::OpenCurly;
        literals[ToUChar('}')] = TokenKind::CloseCurly;

        return literals;
    }

    constexpr auto CharLiterals = MakeCharLiterals();

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

		_STL_UNREACHABLE; 
		return "";
    }

    std::string_view TokenValueToText(const std::string_view& arView)
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

    std::string TokenToText(const Token& aToken)
    {
        return std::format("{} : {} : {{{}, {}}}", TokenKindToText(aToken.Kind), 
			TokenValueToText(aToken.Value), aToken.Loc.Line, aToken.Loc.Col);
    }

	bool StartsIdentifier(unsigned char c)
	{
		return isalpha(c) || c == '_';
	}

	bool ContinuesIdentifier(unsigned char c)
	{
		return isalnum(c) || c == '_';
	}
}

void Lexer::PrintContent()
{
    std::cout << Content << '\n';
}

void Lexer::PrintTokens()
{
    for (const auto& token : Tokens)
    {
        std::cout << TokenToText(token) << '\n';
    }
}

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

void Lexer::Tokenize()
{
    while (QCursorValid())
    {
        ConstructNext();
    }
}

void Lexer::ConstructNext()
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

inline void Lexer::IterateChar()
{
	++Cursor;
}

inline void Lexer::IterateChars(size_t aCount)
{
	Cursor += aCount;
}
