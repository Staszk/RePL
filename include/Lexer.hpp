#ifndef REPL_LEXER_H
#define REPL_LEXER_H

#include "Token.hpp"
#include <string_view>
#include <vector>

class Lexer final
{
public:
    Lexer() = delete;
    Lexer(const char* apText) : Content(apText) { Tokenize(); }
    Lexer(const Lexer&) = delete;
    Lexer(Lexer&&) = delete;
    ~Lexer() = default;

    Lexer& operator=(const Lexer&) = delete;
    Lexer& operator=(Lexer&&) = delete;

    void PrintContent();
    void PrintTokens();

private:
    void ConsumeToken();
    void IterateChar();
    void IterateChars(size_t aCount);
    const char& PeekCursor() const { return Content.at(Cursor); }
    void PushToken(Token&& arrToken) { Tokens.emplace_back(std::move(arrToken)); }
    bool QCursorValid() const { return Cursor < Content.size(); }
    void Tokenize();
    void TrimWhitespace();

    // Input
    std::string_view Content{""};
    // State
    size_t Cursor{};
    size_t LineIdx{};
    size_t StartOfLine{};
    // Output
    std::vector<Token> Tokens{};
};

#endif // REPL_LEXER_H