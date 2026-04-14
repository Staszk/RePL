#ifndef REPL_PARSER_HPP
#define REPL_PARSER_HPP

#include "Lexer.hpp"
#include <memory>

/**
 * @brief Base class for parser AST nodes.
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

/**
 * @brief Recursive descent parser for the RePL language.
 */
class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    
private:
    bool Check(TokenKind type) const;
    Token Consume(TokenKind type, std::string_view message);
    Token Advance();
    bool Match(TokenKind type);
    void Parse();
    Token Peek() const;

    /**
     * @brief Get the parser root AST node.
     *
     * @return A reference to the root AST node pointer.
     */
    std::unique_ptr<ASTNode>& GetRoot() { return Root; }

    // Input
    const std::vector<Token>& Tokens;
    // State
    size_t Current{};
    // Output
    std::unique_ptr<ASTNode> Root{};
};

#endif // REPL_PARSER_HPP