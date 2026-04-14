#include "Parser.hpp"
#include <stdexcept>

/**
 * @brief Construct a new Parser instance.
 *
 * @param arTokens The token stream to parse.
 */
Parser::Parser(const std::vector<Token>& arTokens)
    : Tokens(arTokens), Current(0)
{
    Parse();
}

/**
 * @brief Parse the token stream and build the AST.
 */
void Parser::Parse() 
{
    return;
}

/**
 * @brief Peek at the current token without consuming it.
 *
 * @return The current token.
 */
Token Parser::Peek() const 
{
    if (Current < Tokens.size()) 
    {
        return Tokens[Current];
    }
    return Tokens.back();
}

/**
 * @brief Advance to the next token and return the previous one.
 *
 * @return The token before advancing.
 */
Token Parser::Advance() 
{
    if (Tokens.empty()) 
    {
        throw std::runtime_error("Parser has no tokens to advance.");
    }

    if (Current < Tokens.size()) 
    {
        return Tokens[Current++];
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
    if (Current >= Tokens.size()) 
    {
        return false;
    }
    return Tokens[Current].Kind == akind;
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
Token Parser::Consume(TokenKind akind, std::string_view message) 
{
    if (Check(akind)) 
    {
        return Advance();
    }
    throw std::runtime_error(std::string(message));
}