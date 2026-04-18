#include "ASTPrinter.hpp"
#include <string>
#include <sstream>

/**
 * @brief Print a string representation of the given AST node and its children.
 * 
 * @param node The AST node to print.
 * @return A string representation of the AST node and its children.
 */
std::string ASTPrinter::Print(const class ASTNode& node)
{
    switch (node.Kind)
    {    
    using enum ASTNodeKind;
    case Invalid: return "Invalid";
    case Expr: return Print(static_cast<const ExprNode&>(node));
    case KeywordLiteralExpr: return Print(static_cast<const KeywordLiteralExprNode&>(node));
    case IntLiteralExpr: return Print(static_cast<const IntLiteralExprNode&>(node));
    case StringLiteralExpr: return Print(static_cast<const StringLiteralExprNode&>(node));
    case IdentifierExpr: return Print(static_cast<const IdentifierExprNode&>(node));
    case BinaryExpr: return Print(static_cast<const BinaryExprNode&>(node));
    case UnaryExpr: return Print(static_cast<const UnaryExprNode&>(node));
    case GroupingExpr: return Print(static_cast<const GroupingExprNode&>(node));
    default: return "Unknown ASTNodeKind";
    }

}

/**
 * @brief Print a string representation of the given expression node.
 * 
 * @param node The expression node to print.
 * @return A string representation of the expression node.
 */
std::string ASTPrinter::Print(const class ExprNode& node)
{
    return "";
}

/**
 * @brief Print a string representation of the given keyword literal expression node.
 * 
 * @param node The keyword literal expression node to print.
 * @return A string representation of the keyword literal expression node.
 */
std::string ASTPrinter::Print(const class KeywordLiteralExprNode& node)
{
    return std::string(node.ValueToken.Value);
}

/**
 * @brief Print a string representation of the given integer literal expression node.
 * 
 * @param node The integer literal expression node to print.
 * @return A string representation of the integer literal expression node.
 */
std::string ASTPrinter::Print(const class IntLiteralExprNode& node)
{
    return std::string(node.ValueToken.Value);
}

/**
 * @brief Print a string representation of the given string literal expression node.
 * 
 * @param node The string literal expression node to print.
 * @return A string representation of the string literal expression node.
 */
std::string ASTPrinter::Print(const class StringLiteralExprNode& node)
{
    return std::string(node.ValueToken.Value);
}

/**
 * @brief Print a string representation of the given identifier expression node.
 * 
 * @param node The identifier expression node to print.
 * @return A string representation of the identifier expression node.
 */
std::string ASTPrinter::Print(const class IdentifierExprNode& node)
{
    return std::string(node.ValueToken.Value);
}

/**
 * @brief Print a string representation of the given binary expression node.
 * 
 * @param node The binary expression node to print.
 * @return A string representation of the binary expression node.
 */
std::string ASTPrinter::Print(const class BinaryExprNode& node)
{
    return Parenthesize(node.OperatorToken.Value, { node.Left.get(), node.Right.get() });
}

/**
 * @brief Print a string representation of the given unary expression node.
 * 
 * @param node The unary expression node to print.
 * @return A string representation of the unary expression node.
 */
std::string ASTPrinter::Print(const class UnaryExprNode& node)
{
    return Parenthesize(node.OperatorToken.Value, { node.Operand.get() });
}

/**
 * @brief Print a string representation of the given grouping expression node.
 * 
 * @param node The grouping expression node to print.
 * @return A string representation of the grouping expression node.
 */
std::string ASTPrinter::Print(const class GroupingExprNode& node)
{
    return Parenthesize("group", { node.Inner.get() });
}

/**
 * @brief Parenthesize a string representation of the given node name and its children.
 * 
 * @param name The name of the node.
 * @param children The list of child nodes.
 * @return A string representation of the parenthesized node.
 */
std::string ASTPrinter::Parenthesize(const std::string_view name, std::initializer_list<const ASTNode*> children)
{
    std::ostringstream result;
    result << "(" << name;
    for (const ASTNode* child : children)
    {
        result << " ";
        result << Print(*child);
    }
    result << ")";
    return result.str();
}