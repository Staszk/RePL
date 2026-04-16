#ifndef REPL_PARSER_HPP
#define REPL_PARSER_HPP

#include "AST.hpp"
#include "Lexer.hpp"
#include <memory>

/**
 * @brief Recursive descent parser for the RePL language.
 */
class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    
private:
    bool Check(TokenKind type) const;
    const Token& Consume(TokenKind type, std::string_view message);
    const Token& Advance();
    bool Match(std::initializer_list<TokenKind> aKinds);
    void Parse();
    const Token& Peek(size_t aOffset) const;

    std::unique_ptr<ASTNode> ParseExpression();
    std::unique_ptr<ASTNode> ParseEquality();
    std::unique_ptr<ASTNode> ParseComparison();
    std::unique_ptr<ASTNode> ParseTerm();
    std::unique_ptr<ASTNode> ParseFactor();
    std::unique_ptr<ASTNode> ParseUnary();
    std::unique_ptr<ASTNode> ParsePrimary();

    /**
     * @brief Check if the current token position is valid for parsing.
     * 
     * @return True if the cursor is within the token stream bounds.
     */
    bool CursorValid() const { return Cursor < TokenCount; }

    /**
     * @brief Get the parser root AST node.
     *
     * @return A reference to the root AST node pointer.
     */
    std::unique_ptr<ASTNode>& GetRoot() { return Root; }

    // Input
    const std::vector<Token>& Tokens;
    // State
    const size_t TokenCount{0};
    size_t Cursor{0};
    // Output
    std::unique_ptr<ASTNode> Root{};
};

#endif // REPL_PARSER_HPP