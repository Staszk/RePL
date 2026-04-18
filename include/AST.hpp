#ifndef REPL_AST_HPP
#define REPL_AST_HPP

#include "Token.hpp"
#include <memory>
#include <string_view>
#include <assert.h>

class ASTPrinter;

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
    friend class ASTPrinter;
public:
    ASTNode() noexcept : Kind(ASTNodeKind::Invalid) {}
    ASTNode(ASTNodeKind aKind) noexcept : Kind(aKind) {}
    virtual ~ASTNode() = default;

    virtual std::string Accept(class ASTPrinter* apPrinter) const;

protected:
    ASTNodeKind Kind{};
};

class ExprNode : public ASTNode
{
    friend class ASTPrinter;
public:
    ExprNode() noexcept : ASTNode(ASTNodeKind::Expr) {}
    ExprNode(ASTNodeKind aKind) noexcept : ASTNode(aKind) {}
    
    virtual std::string Accept(class ASTPrinter* apPrinter) const override;
};

class KeywordLiteralExprNode final : public ExprNode
{
    friend class ASTPrinter;
public:
    KeywordLiteralExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::KeywordLiteralExpr), ValueToken(token) {}

    std::string Accept(class ASTPrinter* apPrinter) const override;
private:
    const Token& ValueToken;
};

class IntLiteralExprNode final : public ExprNode
{
    friend class ASTPrinter;
public:
    IntLiteralExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::IntLiteralExpr), ValueToken(token) {}

    std::string Accept(class ASTPrinter* apPrinter) const override;
private:
    const Token& ValueToken;
};

class StringLiteralExprNode final : public ExprNode
{
    friend class ASTPrinter;
public:
    StringLiteralExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::StringLiteralExpr), ValueToken(token) {}

    std::string Accept(class ASTPrinter* apPrinter) const override;
private:
    const Token& ValueToken;
};

class IdentifierExprNode final : public ExprNode
{
    friend class ASTPrinter;
public:
    IdentifierExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::IdentifierExpr), ValueToken(token) {}
    
    std::string Accept(class ASTPrinter* apPrinter) const override;
private:
    const Token& ValueToken;
};

class BinaryExprNode final : public ExprNode
{
    friend class ASTPrinter;
public:
    BinaryExprNode(const Token& token, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
        noexcept : ExprNode(ASTNodeKind::BinaryExpr), OperatorToken(token), Left(std::move(left)), Right(std::move(right)) {}

    std::string Accept(class ASTPrinter* apPrinter) const override;
private:
    const Token& OperatorToken;
    std::unique_ptr<ASTNode> Left;
    std::unique_ptr<ASTNode> Right;
};

class UnaryExprNode final : public ExprNode
{
    friend class ASTPrinter;
public:
    UnaryExprNode(const Token& token, std::unique_ptr<ASTNode> operand)
        noexcept : ExprNode(ASTNodeKind::UnaryExpr), OperatorToken(token), Operand(std::move(operand)) {}

    std::string Accept(class ASTPrinter* apPrinter) const override;
private:
    const Token& OperatorToken;
    std::unique_ptr<ASTNode> Operand;
};

class GroupingExprNode final : public ExprNode
{
    friend class ASTPrinter;
public:
    GroupingExprNode(std::unique_ptr<ASTNode> inner) noexcept : ExprNode(ASTNodeKind::GroupingExpr), Inner(std::move(inner)) {}

    std::string Accept(class ASTPrinter* apPrinter) const override;
private:
    std::unique_ptr<ASTNode> Inner;
};

class StatementNode : public ASTNode
{
    friend class ASTPrinter;
public:
    StatementNode() : ASTNode(ASTNodeKind::Stmnt) {}
};

#endif // REPL_AST_HPP