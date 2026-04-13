#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <string_view>
#include <vector>

enum class TokenKind : uint8_t
{
    End = 0, Invalid,
    Whitespace, NewLine, Tab,
    Keyword, Identifier,
    IntLiteral, StringLiteral, 
    Preprocessor,
    Semicolon, Colon, Comma, OpenParen, CloseParen, OpenCurly, CloseCurly,
};

struct Location
{
    size_t Line;
    size_t Col;
};

struct Token
{
    TokenKind Kind;
    std::string_view Value;
    Location Loc;
};

class Lexer final
{
public:
    Lexer() = delete;
    Lexer(const char* apText) : Content(apText) { Tokenize(); }
    Lexer(const Lexer&) = delete;
    Lexer(Lexer&&) = delete;
    ~Lexer() = default;

    Lexer operator=(const Lexer&) = delete;
    Lexer operator=(Lexer&&) = delete;

    void PrintContent();
    void PrintTokens();

private:
    char PeekCursor() const { return Content.at(Cursor); }
    bool QCursorValid() const { return Cursor < Content.size(); }
    void TrimWhitespace();
    void Tokenize();
    void ConstructNext();
    void IterateChar();
    void IterateChars(size_t aCount);
    void PushToken(Token&& arrToken) { Tokens.emplace_back(std::move(arrToken)); }

    std::string_view Content{ "" };
    std::vector<Token> Tokens{ };
    size_t Cursor{ };
    size_t LineIdx{ };
    size_t StartOfLine{ };
};

#endif