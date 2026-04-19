#include "Parser.hpp"
#include "ASTPrinter.hpp"
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
 * @brief Parse the token stream and build the AST. Any parsing errors 
 * will be caught and printed to stderr. 
 */
void Parser::Parse() 
{
    try
    {
        Root = ParseExpression();
        ASTPrinter printer;
    }
    catch(const ParserError& e)
    {
        std::cerr << std::format("Parser Error: {} at {}\n", e.Message, TokenHelpers::LocationToText(e.Loc));
    }
    catch(const std::exception& e)
    {
        std::cerr << std::format("Uncaptured Parser Exception: {}\n", e.what());
    }
}

/**
 * @brief Peek at the specified token without consuming it.
 *
 * @param aOffset The lookahead offset from the current token. 
 * @return The token at the specified offset.
 */
const Token& Parser::Peek(std::ptrdiff_t aOffset /* = 0 */) const 
{
    assert(Cursor + aOffset >= 0 && "Peek value cannot be negative");

    if (Cursor + aOffset < TokenCount) 
    {
        return Tokens[Cursor + aOffset];
    }
    return Tokens.back();
}

/**
 * @brief Synchronize the parser after an error by advancing until a statement boundary is found.
 */
void Parser::Synchronize()
{
    Advance();
    while (CursorValid())
    {
        if (Peek(-1).Kind == TokenKind::Semicolon || Peek().Kind == TokenKind::KeywordLiteral)
        {
            return;
        }

        Advance();
    }
}

/**
 * @brief Parse an expression from the token stream.
 *
 * @return A unique pointer to the parsed expression AST node.
 */
std::unique_ptr<ASTNode> Parser::ParseExpression()
{
    return ParseEquality();
}

/**
 * @brief Parse an equality expression from the token stream.
 *
 * @return A unique pointer to the parsed equality AST node.
 */
std::unique_ptr<ASTNode> Parser::ParseEquality()
{
    std::unique_ptr<ASTNode> node = ParseComparison();

    while (Match({TokenKind::EqualsEquals, TokenKind::NotEquals}))
    {
        const Token& operatorToken = Peek(-1);
        std::unique_ptr<ASTNode> right = ParseComparison();
        node = std::make_unique<BinaryExprNode>(operatorToken, std::move(node), std::move(right));
    }

    return node;
}

/**
 * @brief Parse a comparison expression from the token stream.
 *
 * @return A unique pointer to the parsed comparison AST node.
 */
std::unique_ptr<ASTNode> Parser::ParseComparison()
{
    std::unique_ptr<ASTNode> node = ParseTerm();

    while (Match({TokenKind::LessEqual, TokenKind::GreaterEqual, TokenKind::Less, TokenKind::Greater}))
    {
        const Token& operatorToken = Peek(-1);
        std::unique_ptr<ASTNode> right = ParseTerm();
        node = std::make_unique<BinaryExprNode>(operatorToken, std::move(node), std::move(right));
    }

    return node;
}

/**
 * @brief Parse a term expression from the token stream.
 *
 * @return A unique pointer to the parsed term AST node.
 */
std::unique_ptr<ASTNode> Parser::ParseTerm()
{
    std::unique_ptr<ASTNode> node = ParseFactor();

    while (Match({TokenKind::Plus, TokenKind::Minus}))
    {
        const Token& operatorToken = Peek(-1);
        std::unique_ptr<ASTNode> right = ParseFactor();
        node = std::make_unique<BinaryExprNode>(operatorToken, std::move(node), std::move(right));
    }

    return node;
}

/**
 * @brief Parse a factor expression from the token stream.
 *
 * @return A unique pointer to the parsed factor AST node.
 */
std::unique_ptr<ASTNode> Parser::ParseFactor()
{
    std::unique_ptr<ASTNode> node = ParseUnary();

    while (Match({TokenKind::Asterisk, TokenKind::Slash, TokenKind::Percent}))
    {
        const Token& operatorToken = Peek(-1);
        std::unique_ptr<ASTNode> right = ParseUnary();
        node = std::make_unique<BinaryExprNode>(operatorToken, std::move(node), std::move(right));
    }

    return node;
}

/**
 * @brief Parse a unary expression from the token stream.
 *
 * @return A unique pointer to the parsed unary AST node.
 */
std::unique_ptr<ASTNode> Parser::ParseUnary()
{
    if (Match({TokenKind::Bang, TokenKind::Minus}))
    {
        const Token& operatorToken = Peek(-1);
        std::unique_ptr<ASTNode> operand = ParseUnary();
        return std::make_unique<UnaryExprNode>(operatorToken, std::move(operand));
    }

    return ParsePrimary();
}

/**
 * @brief Parse a primary expression from the token stream.
 *
 * @return A unique pointer to the parsed primary AST node.
 */
std::unique_ptr<ASTNode> Parser::ParsePrimary()
{
    if (Match({TokenKind::IntLiteral}))
    {
        const Token& intToken = Peek(-1);
        return std::make_unique<IntLiteralExprNode>(intToken);
    }
    else if (Match({TokenKind::FloatLiteral}))
    {
        const Token& floatToken = Peek(-1);
        return std::make_unique<FloatLiteralExprNode>(floatToken);
    }
    else if (Match({TokenKind::StringLiteral}))
    {
        const Token& stringToken = Peek(-1);
        return std::make_unique<StringLiteralExprNode>(stringToken);
    }
    else if (Match({TokenKind::Identifier}))
    {
        const Token& identifierToken = Peek(-1);
        return std::make_unique<IdentifierExprNode>(identifierToken);
    }
    else if (Match({TokenKind::KeywordLiteral}))
    {
        const Token& keywordToken = Peek(-1);
        return std::make_unique<KeywordLiteralExprNode>(keywordToken);
    }
    else if (Match({TokenKind::OpenParen}))
    {
        std::unique_ptr<ASTNode> expr = ParseExpression();
        Consume(TokenKind::CloseParen, "Expected ')' after expression");
        return std::make_unique<GroupingExprNode>(std::move(expr));
    }

    throw GenerateError("Expected expression", Peek().Loc);
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
        throw GenerateError("Parser has no tokens to advance", TokenLocation{0, 0});
    }

    if (CursorValid()) 
    {
        return Tokens[Cursor++];
    }

    return Tokens.back();
}

/**
 * @brief Generate a parser error.
 *
 * @param message The error message.
 * @param loc The location of the error.
 * @return The generated parser error.
 */
const ParserError& Parser::GenerateError(const std::string_view message, const TokenLocation &loc)
{
    Errors.emplace_back( message, loc );
    return Errors.back();
}

/**
 * @brief Check whether the current token is the given kind.
 *
 * @param akind The token kind to compare.
 * @return True when the current token matches the kind.
 */
bool Parser::Check(TokenKind akind) const 
{
    if (!CursorValid()) 
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
bool Parser::Match(std::initializer_list<TokenKind> aKinds) 
{
    for (TokenKind akind : aKinds)
    {
        if (Check(akind)) 
        {
            Advance();
            return true;
        }
    }
    return false;
}

/**
 * @brief Consume a token of the expected kind or generate an error on mismatch.
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

    throw GenerateError(message, Peek().Loc);
}