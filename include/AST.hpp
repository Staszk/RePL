#ifndef REPL_AST_HPP
#define REPL_AST_HPP

#include "Token.hpp"
#include <memory>

/**
 * @brief Kinds of AST nodes in the parser.
 */
enum class ASTNodeKind : uint8_t
{
    Invalid = 0,
    // Expressions
    Expression,
    IntLiteralExpr, FloatLiteralExpr, StringLiteralExpr, CharLiteralExpr,
    IdentifierExpr, BinaryExpr, UnaryExpr, CallExpr,
    // Statements
    Statement,
};

/**
 * @brief Base class for parser AST nodes.
 */
class ASTNode 
{
public:
    ASTNode() noexcept : Kind(ASTNodeKind::Invalid) {}
    ASTNode(ASTNodeKind aKind) noexcept : Kind(aKind) {}
    virtual ~ASTNode() = default;

    ASTNodeKind Kind{};
};

class ExpressionNode : public ASTNode
{
public:
    ExpressionNode() noexcept : ASTNode(ASTNodeKind::Expression) {}
    ExpressionNode(ASTNodeKind aKind) noexcept : ASTNode(aKind) {}

};

class IntLiteralExprNode final : public ExpressionNode
{
public:
    IntLiteralExprNode(const Token& token) noexcept : ExpressionNode(ASTNodeKind::IntLiteralExpr), ValueToken(token) {}

private:
    const Token& ValueToken;
};

class BinaryExprNode final : public ExpressionNode
{
public:
    BinaryExprNode(const Token& token, std::unique_ptr<ExpressionNode> left, std::unique_ptr<ExpressionNode> right)
        noexcept : ExpressionNode(ASTNodeKind::BinaryExpr), OperatorToken(token), Left(std::move(left)), Right(std::move(right)) {}

private:
    const Token& OperatorToken;
    std::unique_ptr<ExpressionNode> Left;
    std::unique_ptr<ExpressionNode> Right;
};


class StatementNode : public ASTNode
{
public:
    StatementNode() : ASTNode(ASTNodeKind::Statement) {}
};

#endif // REPL_AST_HPP