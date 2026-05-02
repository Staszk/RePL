#ifndef REPL_AST_HPP
#define REPL_AST_HPP

#include "ASTPrinter.hpp"
#include "Interpreter.hpp"
#include "Token.hpp"
#include "Keywords.hpp"
#include <memory>
#include <string_view>
#include <assert.h>
#include <iostream>
#include <optional>

template <typename T> struct TypeSpecifier{};

class ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	/**
	 * @brief Accept a visitor to print this expression node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this expression node.
	 */
	virtual std::string Accept(ASTPrinter *apPrinter) const = 0;
	virtual InterpreterValue Accept(Interpreter& apInterpreter) const = 0;
};

class KeywordLiteralExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	KeywordLiteralExprNode(const Token& token) noexcept : ValueToken(token) {}

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

class NumericLiteralExprNode final : public ExprNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	NumericLiteralExprNode(const Token& token) noexcept : ValueToken(token) {}

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
	FloatLiteralExprNode(const Token& token) noexcept : ValueToken(token) {}

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
	StringLiteralExprNode(const Token& token) noexcept : ValueToken(token) {}

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
	IdentifierExprNode(const Token& token) noexcept : ValueToken(token) {}
	
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
	    noexcept : OperatorToken(token), Left(std::move(left)), Right(std::move(right)) {}

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
	    noexcept : OperatorToken(token), Operand(std::move(operand)) {}

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
	GroupingExprNode(std::unique_ptr<ExprNode> inner) noexcept : Inner(std::move(inner)) {}

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
	/**
	 * @brief Accept a visitor to print this statement node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this statement node.
	 */
	virtual std::string Accept(ASTPrinter *apPrinter) const { return "Error\n"; };
	virtual void Accept(Interpreter& apInterpreter) const {};
};

class ExprStmntNode : public StmntNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	ExprStmntNode(std::unique_ptr<ExprNode> aExprNode) : Expression(std::move(aExprNode)) {}
	
	/**
	 * @brief Accept a visitor to print this expression statement node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this expression statement node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return Expression->Accept(apPrinter); }
	void Accept(Interpreter& apInterpreter) const override final { Expression->Accept(apInterpreter); }
	
private:
	std::unique_ptr<ExprNode> Expression;
};

class PrintStmntNode : public StmntNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	PrintStmntNode(std::unique_ptr<ExprNode> aExprNode) : Expression(std::move(aExprNode)) {}
	
	/**
	 * @brief Accept a visitor to print this print statement node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this print statement node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return Expression->Accept(apPrinter); }
	void Accept(Interpreter& apInterpreter) const override final { std::cout << apInterpreter.PrintValue(Expression->Accept(apInterpreter)) << std::endl; }

private:
	std::unique_ptr<ExprNode> Expression;
};

class VarDeclStmntNode : public StmntNode
{
	friend class ASTPrinter;
	friend class Interpreter;
public:
	VarDeclStmntNode(const Token& arToken, uint8_t aSpecifier, bool abIsConst, std::unique_ptr<ExprNode> aExprNode) : IdentifierToken(arToken), Specifier(aSpecifier), IsConst(abIsConst), Expression(std::move(aExprNode)) {}
	
	/**
	 * @brief Accept a visitor to print this print statement node.
	 * 
	 * @param apPrinter The AST printer visitor to accept.
	 * @return A string representation of this print statement node.
	 */
	std::string Accept(ASTPrinter *apPrinter) const override { return Expression->Accept(apPrinter); }
	void Accept(Interpreter& apInterpreter) const override final { apInterpreter.Execute(*this); }

private:
	const Token& IdentifierToken;
	uint8_t Specifier;
	bool IsConst;
	std::unique_ptr<ExprNode> Expression;
};

#endif // REPL_AST_HPP