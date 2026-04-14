#ifndef REPL_LEXER_H
#define REPL_LEXER_H

#include "Token.hpp"
#include <string_view>
#include <vector>

/**
 * @brief Lexical analyzer for the RePL programming language.
 *
 * This class converts input source text into a sequence of tokens
 * that can be consumed by the parser.
 */
class Lexer final
{
public:

    explicit Lexer(std::string_view text);

    void PrintContent();
    void PrintTokens();

    /**
     * @brief Query the generated tokens.
     *
     * @return A const reference to the vector of tokens.
     */
    const std::vector<Token>& tokens() const { return Tokens; }
    const std::vector<Token>& QTokens() const { return tokens(); }

private:
    void Tokenize();
    void TrimWhitespace();
    void ConsumeToken();

    std::pair<TokenKind, size_t> GatherTokenLiteral();

    bool IsNumericLiteralStart();
    bool IsIdentifierStart();
    bool IsPreprocessorDirectiveStart();
    bool IsStringLiteralStart();
    bool IsCharLiteralStart();
    bool IsLineCommentStart();
    bool IsBlockCommentStart();

    void ConsumeTokenLiteral(Token& arToken, TokenKind aKind, size_t aLength);
    void ConsumeNumericLiteralToken(Token& arToken);
    void ConsumeIdentifierToken(Token& arToken);
    void ConsumePreprocessorToken(Token& arToken);
    void ConsumeStringLiteralToken(Token& arToken);
    void ConsumeCharLiteralToken(Token& arToken);
    void ConsumeLineCommentToken(Token& arToken);
    void ConsumeBlockCommentToken(Token& arToken);
    void ConsumeUnknownToken(Token& arToken);

    void Advance();
    void AdvanceBy(size_t aCount);
    char PeekAt(size_t offset) const;

    /**
     * @brief Peek at the current character under the cursor.
     *
     * @return The current character or '\0' if at end of input.
     */
    char PeekCursor() const { return PeekAt(0); }

    /**
     * @brief Add a token to the output token list.
     *
     * @param arrToken The token to push.
     */
    void PushToken(Token&& arrToken) { Tokens.emplace_back(std::move(arrToken)); }

    /**
     * @brief Query whether the cursor is still within input bounds.
     *
     * @return True if the cursor is valid; otherwise false.
     */
    bool CursorValid() const { return Cursor < ContentSize; }

    // Input
    const std::string_view Content{""};
    // State
	const size_t ContentSize{0};
    size_t Cursor{0};
    size_t LineIdx{0};
    size_t StartOfLine{0};
    // Output
    std::vector<Token> Tokens{};
};

#endif // REPL_LEXER_H