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
    Expr,
    IntLiteralExpr, FloatLiteralExpr, StringLiteralExpr, CharLiteralExpr,
    IdentifierExpr, BinaryExpr, UnaryExpr, CallExpr,
    // Statements
    Stmnt,
    ExprStmnt, IfStmnt, ForStmnt, WhileStmnt, ReturnStmnt, ScopeStmnt, VarDeclStmnt,

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

class ExprNode : public ASTNode
{
public:
    ExprNode() noexcept : ASTNode(ASTNodeKind::Expr) {}
    ExprNode(ASTNodeKind aKind) noexcept : ASTNode(aKind) {}

};

class IntLiteralExprNode final : public ExprNode
{
public:
    IntLiteralExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::IntLiteralExpr), ValueToken(token) {}

private:
    const Token& ValueToken;
};

class BinaryExprNode final : public ExprNode
{
public:
    BinaryExprNode(const Token& token, std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right)
        noexcept : ExprNode(ASTNodeKind::BinaryExpr), OperatorToken(token), Left(std::move(left)), Right(std::move(right)) {}

private:
    const Token& OperatorToken;
    std::unique_ptr<ExprNode> Left;
    std::unique_ptr<ExprNode> Right;
};

class StatementNode : public ASTNode
{
public:
    StatementNode() : ASTNode(ASTNodeKind::Stmnt) {}
};

#endif // REPL_AST_HPP