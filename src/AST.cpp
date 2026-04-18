#include "AST.hpp"
#include "ASTPrinter.hpp"

/**
 * @brief Accept a visitor to print this AST node.
 * 
 * @param apPrinter The AST printer visitor to accept.
 * @return A string representation of this AST node.
 */
std::string ASTNode::Accept(ASTPrinter *apPrinter) const
{
    return apPrinter->Print(*this);
}

/**
 * @brief Accept a visitor to print this expression node.
 * 
 * @param apPrinter The AST printer visitor to accept.
 * @return A string representation of this expression node.
 */
std::string ExprNode::Accept(ASTPrinter *apPrinter) const
{
    return apPrinter->Print(*this);
}

/**
 * @brief Accept a visitor to print this keyword literal expression node.
 * 
 * @param apPrinter The AST printer visitor to accept.
 * @return A string representation of this keyword literal expression node.
 */
std::string KeywordLiteralExprNode::Accept(ASTPrinter *apPrinter) const
{
    return apPrinter->Print(*this);
}

/**
 * @brief Accept a visitor to print this integer literal expression node.
 * 
 * @param apPrinter The AST printer visitor to accept.
 * @return A string representation of this integer literal expression node.
 */
std::string IntLiteralExprNode::Accept(ASTPrinter *apPrinter) const
{
    return apPrinter->Print(*this);
}

/**
 * @brief Accept a visitor to print this string literal expression node.
 * 
 * @param apPrinter The AST printer visitor to accept.
 * @return A string representation of this string literal expression node.
 */
std::string StringLiteralExprNode::Accept(ASTPrinter *apPrinter) const
{
    return apPrinter->Print(*this);
}

/**
 * @brief Accept a visitor to print this identifier expression node.
 * 
 * @param apPrinter The AST printer visitor to accept.
 * @return A string representation of this identifier expression node.
 */
std::string IdentifierExprNode::Accept(ASTPrinter *apPrinter) const
{
    return apPrinter->Print(*this);
}

/**
 * @brief Accept a visitor to print this binary expression node.
 * 
 * @param apPrinter The AST printer visitor to accept.
 * @return A string representation of this binary expression node.
 */
std::string BinaryExprNode::Accept(ASTPrinter *apPrinter) const
{
    return apPrinter->Print(*this);
}

/**
 * @brief Accept a visitor to print this unary expression node.
 * 
 * @param apPrinter The AST printer visitor to accept.
 * @return A string representation of this unary expression node.
 */
std::string UnaryExprNode::Accept(ASTPrinter *apPrinter) const
{
    return apPrinter->Print(*this);
}

/**
 * @brief Accept a visitor to print this grouping expression node.
 * 
 * @param apPrinter The AST printer visitor to accept.
 * @return A string representation of this grouping expression node.
 */
std::string GroupingExprNode::Accept(ASTPrinter *apPrinter) const
{
    return apPrinter->Print(*this);
}
