#include "Parser.hpp"
#include <stdexcept>
#include <iostream>

/**
 * @brief Construct a new Parser instance.
 *
 * @param arTokens The token stream to parse.
 */
Parser::Parser(const std::vector<Token>& arTokens)
    : Tokens(arTokens), TokenCount(arTokens.size())
{
    Parse();
}

/**
 * @brief Parse the token stream and build the AST.
 */
void Parser::Parse() 
{
    while (CursorValid()) 
    {
        const Token& currentToken = Tokens[Cursor];
        if (currentToken.Kind == TokenKind::IntLiteral)
        {
            IntLiteralExprNode intNode(currentToken);
            // Here you would typically add intNode to the AST
        }
        Advance();
    }
}

/**
 * @brief Peek at the specified token without consuming it.
 *
 * @param aOffset The lookahead offset from the current token.
 * @return The token at the specified offset.
 */
const Token& Parser::Peek(size_t aOffset) const 
{
    if (Cursor + aOffset < TokenCount) 
    {
        return Tokens[Cursor + aOffset];
    }
    return Tokens.back();
}

/**
 * @brief Advance to the next token and return the previous one.
 *
 * @return The token before advancing.
 */
const Token& Parser::Advance() 
{
    if (Tokens.empty()) 
    {
        throw std::runtime_error("Parser has no tokens to advance.");
    }

    if (CursorValid()) 
    {
        return Tokens[Cursor++];
    }

    return Tokens.back();
}

/**
 * @brief Check whether the current token is the given kind.
 *
 * @param akind The token kind to compare.
 * @return True when the current token matches the kind.
 */
bool Parser::Check(TokenKind akind) const 
{
    if (CursorValid()) 
    {
        return false;
    }
    return Tokens[Cursor].Kind == akind;
}

/**
 * @brief Match and consume a token of the specified kind.
 *
 * @param akind The token kind to match.
 * @return True when the token matched and was consumed.
 */
bool Parser::Match(TokenKind akind) 
{
    if (Check(akind)) 
    {
        Advance();
        return true;
    }
    return false;
}

/**
 * @brief Consume a token of the expected kind or throw on mismatch.
 *
 * @param akind The expected token kind.
 * @param message The error message to use if consumption fails.
 * @return The consumed token.
 */
const Token& Parser::Consume(TokenKind akind, std::string_view message) 
{
    if (Check(akind)) 
    {
        return Advance();
    }
    throw std::runtime_error(std::string(message));
}