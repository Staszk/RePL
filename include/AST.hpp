#ifndef REPL_AST_HPP
#define REPL_AST_HPP

#include "ASTPrinter.hpp"
#include "Interpreter.hpp"
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
	friend class ASTPrinter;
	friend class Interpreter;
public:
	ASTNode() noexcept : Kind(ASTNodeKind::Invalid) {}
	ASTNode(ASTNodeKind aKind) noexcept : Kind(aKind) {}
	virtual ~ASTNode() = default;

	/**
	 * @brief Accept a visitor to print this node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this node.
	 */
	virtual std::string Accept(ASTPrinter *apPrinter) const { return apPrinter->Print(*this); }
	virtual InterpreterValue Accept(Interpreter& apInterpreter) const { return apInterpreter.Interpret(*this); }

protected:
	ASTNodeKind Kind{};
};

class ExprNode : public ASTNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	ExprNode() noexcept : ASTNode(ASTNodeKind::Expr) {}
	ExprNode(ASTNodeKind aKind) noexcept : ASTNode(aKind) {}
	
	/**
	 * @brief Accept a visitor to print this expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this expression node.
	 */
	virtual std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	virtual InterpreterValue Accept(Interpreter& apInterpreter) const { return apInterpreter.Interpret(*this); }
};

class KeywordLiteralExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	KeywordLiteralExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::KeywordLiteralExpr), ValueToken(token) {}

	/**
	 * @brief Accept a visitor to print this keyword literal expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this keyword literal expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override { return apInterpreter.Interpret(*this); }
private:
	const Token& ValueToken;
};

class IntLiteralExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	IntLiteralExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::IntLiteralExpr), ValueToken(token) {}

	/**
	 * @brief Accept a visitor to print this integer literal expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this integer literal expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override { return apInterpreter.Interpret(*this); }
private:
	const Token& ValueToken;
};

class FloatLiteralExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	FloatLiteralExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::FloatLiteralExpr), ValueToken(token) {}

	/**
	 * @brief Accept a visitor to print this float literal expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this float literal expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override { return apInterpreter.Interpret(*this); }

private:
	const Token& ValueToken;
};

class StringLiteralExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	StringLiteralExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::StringLiteralExpr), ValueToken(token) {}

	/**
	 * @brief Accept a visitor to print this string literal expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this string literal expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override { return apInterpreter.Interpret(*this); }
private:
	const Token& ValueToken;
};

class IdentifierExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	IdentifierExprNode(const Token& token) noexcept : ExprNode(ASTNodeKind::IdentifierExpr), ValueToken(token) {}
	
	/**
	 * @brief Accept a visitor to print this identifier expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this identifier expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override { return apInterpreter.Interpret(*this); }
private:
	const Token& ValueToken;
};

class BinaryExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	BinaryExprNode(const Token& token, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
	    noexcept : ExprNode(ASTNodeKind::BinaryExpr), OperatorToken(token), Left(std::move(left)), Right(std::move(right)) {}

	/**
	 * @brief Accept a visitor to print this binary expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this binary expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override { return apInterpreter.Interpret(*this); }
private:
	const Token& OperatorToken;
	std::unique_ptr<ASTNode> Left;
	std::unique_ptr<ASTNode> Right;
};

class UnaryExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	UnaryExprNode(const Token& token, std::unique_ptr<ASTNode> operand)
	    noexcept : ExprNode(ASTNodeKind::UnaryExpr), OperatorToken(token), Operand(std::move(operand)) {}

	/**
	 * @brief Accept a visitor to print this unary expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this unary expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override { return apInterpreter.Interpret(*this); }
private:
	const Token& OperatorToken;
	std::unique_ptr<ASTNode> Operand;
};

class GroupingExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	GroupingExprNode(std::unique_ptr<ASTNode> inner) noexcept : ExprNode(ASTNodeKind::GroupingExpr), Inner(std::move(inner)) {}

	/**
	 * @brief Accept a visitor to print this grouping expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this grouping expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override { return apInterpreter.Interpret(*this); }
private:
	std::unique_ptr<ASTNode> Inner;
};

class StatementNode : public ASTNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	StatementNode() : ASTNode(ASTNodeKind::Stmnt) {}
};

#endif // REPL_AST_HPP