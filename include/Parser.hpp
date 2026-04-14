#ifndef REPL_PARSER_HPP
#define REPL_PARSER_HPP

#include "Lexer.hpp"

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

public:
    Parser(const std::vector<Token>& tokens);
    
    std::unique_ptr<ASTNode> Parse();
    
private:
    Token Peek() const;
    Token Advance();
    bool Check(TokenType type) const;
    bool Match(TokenType type);
    Token Consume(TokenType type, const std::string_view message);
};

#endif // REPLPARSER_HPP