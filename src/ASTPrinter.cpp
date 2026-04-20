#include "ASTPrinter.hpp"
#include "AST.hpp"
#include <string>
#include <sstream>

/**
 * @brief Print a string representation of the given AST node and its children.
 * 
 * @param node UNUSED: The AST node to print.
 * @return A string representation of the AST node and its children.
 */
std::string ASTPrinter::Print(const class ASTNode&)
{
	return "";
}

/**
 * @brief Print a string representation of the given expression node.
 * 
 * @param node UNUSED: The expression node to print.
 * @return A string representation of the expression node.
 */
std::string ASTPrinter::Print(const class ExprNode&)
{
	return "";
}

/**
 * @brief Print a string representation of the given keyword literal expression node.
 * 
 * @param arNode The keyword literal expression node to print.
 * @return A string representation of the keyword literal expression node.
 */
std::string ASTPrinter::Print(const class KeywordLiteralExprNode& arNode)
{
	return std::string(arNode.ValueToken.Value);
}

/**
 * @brief Print a string representation of the given integer literal expression node.
 * 
 * @param arNode The integer literal expression node to print.
 * @return A string representation of the integer literal expression node.
 */
std::string ASTPrinter::Print(const class IntLiteralExprNode& arNode)
{
	return std::string(arNode.ValueToken.Value);
}

/**
 * @brief Print a string representation of the given float literal expression node.
 * 
 * @param arNode The float literal expression node to print.
 * @return A string representation of the float literal expression node.
 */
std::string ASTPrinter::Print(const class FloatLiteralExprNode& arNode)
{
	return std::string(arNode.ValueToken.Value);
}

/**
 * @brief Print a string representation of the given string literal expression node.
 * 
 * @param arNode The string literal expression node to print.
 * @return A string representation of the string literal expression node.
 */
std::string ASTPrinter::Print(const class StringLiteralExprNode& arNode)
{
	return std::string(arNode.ValueToken.Value);
}

/**
 * @brief Print a string representation of the given identifier expression node.
 * 
 * @param arNode The identifier expression node to print.
 * @return A string representation of the identifier expression node.
 */
std::string ASTPrinter::Print(const class IdentifierExprNode& arNode)
{
	return std::string(arNode.ValueToken.Value);
}

/**
 * @brief Print a string representation of the given binary expression node.
 * 
 * @param arNode The binary expression node to print.
 * @return A string representation of the binary expression node.
 */
std::string ASTPrinter::Print(const class BinaryExprNode& arNode)
{
	return Parenthesize(arNode.OperatorToken.Value, { arNode.Left.get(), arNode.Right.get() });
}

/**
 * @brief Print a string representation of the given unary expression node.
 * 
 * @param arNode The unary expression node to print.
 * @return A string representation of the unary expression node.
 */
std::string ASTPrinter::Print(const class UnaryExprNode& arNode)
{
	return Parenthesize(arNode.OperatorToken.Value, { arNode.Operand.get() });
}

/**
 * @brief Print a string representation of the given grouping expression node.
 * 
 * @param arNode The grouping expression node to print.
 * @return A string representation of the grouping expression node.
 */
std::string ASTPrinter::Print(const class GroupingExprNode& arNode)
{
	return Parenthesize("group", { arNode.Inner.get() });
}

/**
 * @brief Parenthesize a string representation of the given node name and its children.
 * 
 * @param aName The name of the node.
 * @param aChildren The list of child nodes.
 * @return A string representation of the parenthesized node.
 */
std::string ASTPrinter::Parenthesize(const std::string_view aName, std::initializer_list<const ASTNode*> aChildren)
{
	std::ostringstream result;
	result << "(" << aName;
	for (const ASTNode* child : aChildren)
	{
	    result << " ";
	    result << child->Accept(this);
	}
	result << ")";
	return result.str();
}