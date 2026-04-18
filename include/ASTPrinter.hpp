#ifndef REPL_ASTPRINTER_HPP
#define REPL_ASTPRINTER_HPP

#include "AST.hpp"

class ASTPrinter
{
public:
    std::string Print(const class ASTNode& node);
    std::string Print(const class ExprNode& node);
    std::string Print(const class KeywordLiteralExprNode& node);
    std::string Print(const class IntLiteralExprNode& node);
    std::string Print(const class StringLiteralExprNode& node);
    std::string Print(const class IdentifierExprNode& node);
    std::string Print(const class BinaryExprNode& node);
    std::string Print(const class UnaryExprNode& node);
    std::string Print(const class GroupingExprNode& node);
    std::string Parenthesize(const std::string_view name, std::initializer_list<const ASTNode*> children);

};

#endif // REPL_ASTPRINTER_HPP