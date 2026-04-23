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
enum class ExprNodeKind : uint8_t
{
	Invalid = 0,
	Expr,
	KeywordLiteralExpr, IntLiteralExpr, FloatLiteralExpr, StringLiteralExpr, CharLiteralExpr,
	IdentifierExpr, BinaryExpr, UnaryExpr, GroupingExpr,
	
};

enum class StmntNodeKind : uint8_t
{
	Invalid = 0,
	Stmnt,
	ExprStmnt, IfStmnt, ForStmnt, WhileStmnt, ReturnStmnt, ScopeStmnt, VarDeclStmnt,

};

constexpr std::string_view ExprNodeKindToText(ExprNodeKind aKind)
{
	switch (aKind)
	{
		using enum ExprNodeKind;
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
	case GroupingExpr: return "GroupingExpr";
	}

	assert(false && "Invalid ExprNodeKind");
	return "";
}

constexpr std::string_view StmntNodeKindToText(StmntNodeKind aKind)
{
	switch(aKind)
	{
		using enum StmntNodeKind;
	case Stmnt: return "Stmnt";
	case ExprStmnt: return "ExprStmnt";
	case IfStmnt: return "IfStmnt";
	case ForStmnt: return "ForStmnt";
	case WhileStmnt: return "WhileStmnt";
	case ReturnStmnt: return "ReturnStmnt";
	case ScopeStmnt: return "ScopeStmnt";
	case VarDeclStmnt: return "VarDeclStmnt";
	}

	assert(false && "Invalid StmntNodeKind");
	return "";
}

class ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	ExprNode() noexcept : Kind(ExprNodeKind::Expr) {}
	ExprNode(ExprNodeKind aKind) noexcept : Kind(aKind) {}
	
	/**
	 * @brief Accept a visitor to print this expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this expression node.
	 */
	virtual std::string Accept(ASTPrinter *apPrinter) const = 0;
	virtual InterpreterValue Accept(Interpreter& apInterpreter) const = 0;

protected:
	ExprNodeKind Kind;
};

class KeywordLiteralExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	KeywordLiteralExprNode(const Token& token) noexcept : ExprNode(ExprNodeKind::KeywordLiteralExpr), ValueToken(token) {}

	/**
	 * @brief Accept a visitor to print this keyword literal expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this keyword literal expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override final { return apInterpreter.Interpret(*this); }
private:
	const Token& ValueToken;
};

class IntLiteralExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	IntLiteralExprNode(const Token& token) noexcept : ExprNode(ExprNodeKind::IntLiteralExpr), ValueToken(token) {}

	/**
	 * @brief Accept a visitor to print this integer literal expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this integer literal expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override final { return apInterpreter.Interpret(*this); }
private:
	const Token& ValueToken;
};

class FloatLiteralExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	FloatLiteralExprNode(const Token& token) noexcept : ExprNode(ExprNodeKind::FloatLiteralExpr), ValueToken(token) {}

	/**
	 * @brief Accept a visitor to print this float literal expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this float literal expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override final { return apInterpreter.Interpret(*this); }

private:
	const Token& ValueToken;
};

class StringLiteralExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	StringLiteralExprNode(const Token& token) noexcept : ExprNode(ExprNodeKind::StringLiteralExpr), ValueToken(token) {}

	/**
	 * @brief Accept a visitor to print this string literal expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this string literal expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override final { return apInterpreter.Interpret(*this); }
private:
	const Token& ValueToken;
};

class IdentifierExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	IdentifierExprNode(const Token& token) noexcept : ExprNode(ExprNodeKind::IdentifierExpr), ValueToken(token) {}
	
	/**
	 * @brief Accept a visitor to print this identifier expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this identifier expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override final { return apInterpreter.Interpret(*this); }
private:
	const Token& ValueToken;
};

class BinaryExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	BinaryExprNode(const Token& token, std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right)
	    noexcept : ExprNode(ExprNodeKind::BinaryExpr), OperatorToken(token), Left(std::move(left)), Right(std::move(right)) {}

	/**
	 * @brief Accept a visitor to print this binary expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this binary expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override final { return apInterpreter.Interpret(*this); }
private:
	const Token& OperatorToken;
	std::unique_ptr<ExprNode> Left;
	std::unique_ptr<ExprNode> Right;
};

class UnaryExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	UnaryExprNode(const Token& token, std::unique_ptr<ExprNode> operand)
	    noexcept : ExprNode(ExprNodeKind::UnaryExpr), OperatorToken(token), Operand(std::move(operand)) {}

	/**
	 * @brief Accept a visitor to print this unary expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this unary expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override final { return apInterpreter.Interpret(*this); }
private:
	const Token& OperatorToken;
	std::unique_ptr<ExprNode> Operand;
};

class GroupingExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	GroupingExprNode(std::unique_ptr<ExprNode> inner) noexcept : ExprNode(ExprNodeKind::GroupingExpr), Inner(std::move(inner)) {}

	/**
	 * @brief Accept a visitor to print this grouping expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this grouping expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return apPrinter->Print(*this); }
	InterpreterValue Accept(Interpreter& apInterpreter) const override final { return apInterpreter.Interpret(*this); }
private:
	std::unique_ptr<ExprNode> Inner;
};

class StmntNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	StmntNode() noexcept : Kind(StmntNodeKind::Stmnt) {}
	StmntNode(StmntNodeKind aKind) noexcept : Kind(aKind) {}
	
	/**
	 * @brief Accept a visitor to print this statement node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this statement node.
	 */
	virtual std::string Accept(ASTPrinter *apPrinter) const = 0;
	//virtual InterpreterValue Accept(Interpreter& apInterpreter) const = 0;

protected:
	StmntNodeKind Kind;
};

class ExprStmntNode : public StmntNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	ExprStmntNode(std::unique_ptr<ExprNode> aExprNode) : StmntNode(StmntNodeKind::ExprStmnt), Expression(std::move(aExprNode)) {}
	
	/**
	 * @brief Accept a visitor to print this grouping expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this grouping expression node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return Expression->Accept(apPrinter); }
	//InterpreterValue Accept(Interpreter& apInterpreter) const override final { return apInterpreter.Interpret(*this); }
	
private:
	std::unique_ptr<ExprNode> Expression;
};

#endif // REPL_AST_HPP