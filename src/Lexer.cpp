#include "Lexer.hpp"
#include <format>
#include <unordered_map>
#include <iostream>

#define TRIM_WHITESPACE 1

#define MAX_BUFFER = 128;

namespace 
{
    const std::unordered_map<char, TokenKind> CharLiterals =
    {
        { '\0', TokenKind::End },
        { ' ',  TokenKind::Whitespace },
        { '\n',  TokenKind::NewLine },
        { '\t',  TokenKind::Tab },
        { ';',  TokenKind::Semicolon },
        { ':',  TokenKind::Colon },
        { ',',  TokenKind::Comma },
        { '(',  TokenKind::OpenParen },
        { ')',  TokenKind::CloseParen },
        { '{',  TokenKind::OpenCurly },
        { '}',  TokenKind::CloseCurly },
    };

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

	bool StartsIdentifier(char c)
	{
		return isalpha(c) || c == '_';
	}

	bool ContinuesIdentifier(char c)
	{
		return isalnum(c) || c == '_';
	}
}

void Lexer::PrintContent()
{
    std::puts(std::format("{}", Content).data());
}

void Lexer::PrintTokens()
{
    for (auto& t : Tokens)
    {
        std::puts(TokenToText(t).data());
    }
}

void Lexer::TrimWhitespace()
{
    while (QCursorValid() && isspace( Content.at(Cursor) ))
	{
		if (Content.at(Cursor) == '\n')
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
#if TRIM_WHITESPACE
	TrimWhitespace();
#endif

    Token token{ };

    const char c = Content.at(Cursor);

    if (CharLiterals.contains(c)) 
    {
        token.Kind = CharLiterals.at(c);
        token.Value = Content.substr(Cursor, 1);
		token.Loc = { LineIdx, Cursor - StartOfLine };

#if TRIM_WHITESPACE
        if (token.Kind == TokenKind::NewLine)
        {
            ++LineIdx;
            StartOfLine = Cursor + 1;
        }
#endif
		IterateChar();
    }
    else if (isdigit(c))
    {
        token.Kind = TokenKind::IntLiteral;
        size_t begin = Cursor;
        while(QCursorValid() && isdigit(PeekCursor())) IterateChar();
        token.Value = Content.substr(begin, Cursor - begin);
		token.Loc = { LineIdx, begin - StartOfLine };
    }
    else if (StartsIdentifier(c))
    {
        token.Kind = TokenKind::Identifier;
        size_t begin = Cursor;
        while(QCursorValid() && ContinuesIdentifier(PeekCursor())) IterateChar();
        token.Value = Content.substr(begin, Cursor - begin);
		token.Loc = { LineIdx, begin - StartOfLine };
    }
    else if (c == '#')
    {
        token.Kind = TokenKind::Preprocessor;
        size_t begin = Cursor;
        while(QCursorValid() && PeekCursor() != '\n') IterateChar();
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
