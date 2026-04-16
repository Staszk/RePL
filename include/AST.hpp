#ifndef REPL_AST_HPP
#define REPL_AST_HPP

#include "Token.hpp"
#include <memory>
#include <string_view>
#include <assert.h>

/**
 * @brief Kinds of AST nodes in the parser.
 */
enum class ASTNodeKind : uint8_t
{
    Invalid = 0,
    // Expressions
    Expr,
    KeywordLiteralExpr, IntLiteralExpr, FloatLiteralExpr, StringLiteralExpr, CharLiteralExpr,
    IdentifierExpr, BinaryExpr, UnaryExpr, CallExpr,
    // Grouping
    GroupingExpr,
    // Statements
    Stmnt,
    ExprStmnt, IfStmnt, ForStmnt, WhileStmnt, ReturnStmnt, ScopeStmnt, VarDeclStmnt,

};

constexpr std::string_view ASTNodeKindToText(ASTNodeKind aKind)
{
    switch (aKind)
    {
    using enum ASTNodeKind;
    case Invalid: return "Invalid";
    case Expr: return "Expr";
    case KeywordLiteralExpr: return "KeywordLiteralExpr";
    case IntLiteralExpr: return "IntLiteralExpr";
    case FloatLiteralExpr: return "FloatLiteralExpr";
    case StringLiteralExpr: return "StringLiteralExpr";
    case CharLiteralExpr: return "CharLiteralExpr";
    case IdentifierExpr: return "IdentifierExpr";
    case BinaryExpr: return "BinaryExpr";
    case UnaryExpr: return "UnaryExpr";
    case CallExpr: return "CallExpr";
    case GroupingExpr: return "GroupingExpr";
    case Stmnt: return "Stmnt";
    case ExprStmnt: return "ExprStmnt";
    case IfStmnt: return "IfStmnt";
    case ForStmnt: return "ForStmnt";
    case WhileStmnt: return "WhileStmnt";
    case ReturnStmnt: return "ReturnStmnt";
    case ScopeStmnt: return "ScopeStmnt";
    case VarDeclStmnt: return "VarDeclStmnt";
    }

    assert(false && "Invalid ASTNodeKind");
    return "";
}

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

class KeywordLiteralExprNode final : public ExprNode
{
public:
    KeywordLiteralExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::KeywordLiteralExpr), ValueToken(token) {}

private:
    const Token& ValueToken;
};

class IntLiteralExprNode final : public ExprNode
{
public:
    IntLiteralExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::IntLiteralExpr), ValueToken(token) {}

private:
    const Token& ValueToken;
};

class StringLiteralExprNode final : public ExprNode
{
public:
    StringLiteralExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::StringLiteralExpr), ValueToken(token) {}

private:
    const Token& ValueToken;
};

class IdentifierExprNode final : public ExprNode
{
public:
    IdentifierExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::IdentifierExpr), NameToken(token) {}
    
private:
    const Token& NameToken;
};

class BinaryExprNode final : public ExprNode
{
public:
    BinaryExprNode(const Token& token, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
        noexcept : ExprNode(ASTNodeKind::BinaryExpr), OperatorToken(token), Left(std::move(left)), Right(std::move(right)) {}

private:
    const Token& OperatorToken;
    std::unique_ptr<ASTNode> Left;
    std::unique_ptr<ASTNode> Right;
};

class UnaryExprNode final : public ExprNode
{
public:
    UnaryExprNode(const Token& token, std::unique_ptr<ASTNode> operand)
        noexcept : ExprNode(ASTNodeKind::UnaryExpr), OperatorToken(token), Operand(std::move(operand)) {}

private:
    const Token& OperatorToken;
    std::unique_ptr<ASTNode> Operand;
};

class GroupingExprNode final : public ExprNode
{
public:
    GroupingExprNode(std::unique_ptr<ASTNode> inner) noexcept : ExprNode(ASTNodeKind::GroupingExpr), Inner(std::move(inner)) {}
private:
    std::unique_ptr<ASTNode> Inner;
};

class StatementNode : public ASTNode
{
public:
    StatementNode() : ASTNode(ASTNodeKind::Stmnt) {}
};

#endif // REPL_AST_HPP