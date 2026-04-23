#include "Parser.hpp"
#include "ASTPrinter.hpp"
#include <stdexcept>
#include <iostream>

namespace
{
	bool IsAdjacent(const Token& aFirst, const Token& aSecond)
	{
		if (aFirst.Loc.Line != aSecond.Loc.Line) return false;

		return (aFirst.Loc.Col + aFirst.Value.size()) == aSecond.Loc.Col;
	}
}

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
		while (CursorValid())
		{
			Program.emplace_back(ParseStatement());
		}
	}
	catch(const ParserError& e)
	{
		std::cout << std::format("{} Error: {} at {}\n", e.ErrorType == ParserError::Type::Parser ? "Parser" : "Syntax", e.Message, TokenHelpers::LocationToText(e.Loc));
	}
	catch(const std::exception& e)
	{
		std::cout << std::format("Uncaptured Parser Exception: {}\n", e.what());
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
	assert(Cursor + aOffset >= 0 && "Peek index cannot be negative");

	if (Cursor + aOffset < TokenCount) 
	{
		return Tokens[Cursor + aOffset];
	}
	return Tokens.back();
}

void Parser::RequireWhitespace(const std::string_view aMessage, std::ptrdiff_t aPreviousOffset /* = -1 */, std::ptrdiff_t aCurrentOffset /* = 0 */)
{
	const Token& previous = Peek(aPreviousOffset);
	const Token& current = Peek(aCurrentOffset);
	if (IsAdjacent(previous, current))
	{
		throw GenerateError(aMessage, current.Loc, ParserError::Type::Syntax);
	}
}

const Token &Parser::RetrieveBinaryOperator()
{
	if (Check(TokenKind::End)) throw GenerateError("Expected expression", Peek().Loc);
	RequireWhitespace("RePL requires whitespace before binary operator", -2, -1);
	RequireWhitespace("RePL requires whitespace after binary operator");
	return Peek(-1);
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
std::unique_ptr<ExprNode> Parser::ParseExpression()
{
	return ParseEquality();
}

/**
 * @brief Parse an equality expression from the token stream.
 *
 * @return A unique pointer to the parsed equality AST node.
 */
std::unique_ptr<ExprNode> Parser::ParseEquality()
{
	std::unique_ptr<ExprNode> node = ParseComparison();

	while (Match({TokenKind::EqualEqual, TokenKind::NotEqual}))
	{
		const Token& operatorToken = RetrieveBinaryOperator();
		std::unique_ptr<ExprNode> right = ParseComparison();
		node = std::make_unique<BinaryExprNode>(operatorToken, std::move(node), std::move(right));
	}

	return node;
}

/**
 * @brief Parse a comparison expression from the token stream.
 *
 * @return A unique pointer to the parsed comparison AST node.
 */
std::unique_ptr<ExprNode> Parser::ParseComparison()
{
	std::unique_ptr<ExprNode> node = ParseTerm();

	while (Match({TokenKind::LesserEqual, TokenKind::GreaterEqual, TokenKind::Lesser, TokenKind::Greater}))
	{
		const Token& operatorToken = RetrieveBinaryOperator();
		std::unique_ptr<ExprNode> right = ParseTerm();
		node = std::make_unique<BinaryExprNode>(operatorToken, std::move(node), std::move(right));
	}

	return node;
}

/**
 * @brief Parse a term expression from the token stream.
 *
 * @return A unique pointer to the parsed term AST node.
 */
std::unique_ptr<ExprNode> Parser::ParseTerm()
{
	std::unique_ptr<ExprNode> node = ParseFactor();

	while (Match({TokenKind::Plus, TokenKind::Minus}))
	{
		const Token& operatorToken = RetrieveBinaryOperator();
		std::unique_ptr<ExprNode> right = ParseFactor();
		node = std::make_unique<BinaryExprNode>(operatorToken, std::move(node), std::move(right));
	}

	return node;
}

/**
 * @brief Parse a factor expression from the token stream.
 *
 * @return A unique pointer to the parsed factor AST node.
 */
std::unique_ptr<ExprNode> Parser::ParseFactor()
{
	std::unique_ptr<ExprNode> node = ParseUnary();

	while (Match({TokenKind::Asterisk, TokenKind::Slash, TokenKind::Percent}))
	{
		const Token& operatorToken = RetrieveBinaryOperator();
		std::unique_ptr<ExprNode> right = ParseUnary();
		node = std::make_unique<BinaryExprNode>(operatorToken, std::move(node), std::move(right));
	}

	return node;
}

/**
 * @brief Parse a unary expression from the token stream.
 *
 * @return A unique pointer to the parsed unary AST node.
 */
std::unique_ptr<ExprNode> Parser::ParseUnary()
{
	if (Match({TokenKind::Bang, TokenKind::Minus}))
	{
		const Token& operatorToken = Peek(-1);
		std::unique_ptr<ExprNode> operand = ParseUnary();
		return std::make_unique<UnaryExprNode>(operatorToken, std::move(operand));
	}

	return ParsePrimary();
}

/**
 * @brief Parse a primary expression from the token stream.
 *
 * @return A unique pointer to the parsed primary AST node.
 */
std::unique_ptr<ExprNode> Parser::ParsePrimary()
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
		RequireWhitespace("RePL requires whitespace after '('");
		std::unique_ptr<ExprNode> expr = ParseExpression();
		RequireWhitespace("RePL requires whitespace before ')'");
		Consume(TokenKind::CloseParen, "Expected ')' after expression");
		return std::make_unique<GroupingExprNode>(std::move(expr));
	}

	throw GenerateError("Expected expression", Peek().Loc);
}

std::unique_ptr<StmntNode> Parser::ParseStatement()
{
	if (Match({TokenKind::KeywordLiteral}))
	{
		if (Peek(-1).Value == "print")
		{
			return ParsePrintStmnt();
		}
	}

	return ParseExprStmnt();
}

std::unique_ptr<StmntNode> Parser::ParseExprStmnt()
{
	std::unique_ptr<ExprNode> expr = ParseExpression();
	Consume(TokenKind::Semicolon, "Expect ';' after expression");
	return std::make_unique<ExprStmntNode>(std::move(expr));
}

std::unique_ptr<StmntNode> Parser::ParsePrintStmnt()
{
	RequireWhitespace("RePL requires whitespace after after keyword");
	if (!Check(TokenKind::OpenParen)) throw GenerateError("\"print\" must be followed by grouping.", Peek().Loc);
	std::unique_ptr<ExprNode> expr = ParseExpression();
	Consume(TokenKind::Semicolon, "Expect ';' after expression");
	return std::make_unique<PrintStmntNode>(std::move(expr));
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
 * @param aMessage The error message.
 * @param arLoc The location of the error.
 * @param aType The error type.
 * @return The generated parser error.
 */
ParserError Parser::GenerateError(const std::string_view aMessage, const TokenLocation &arLoc, ParserError::Type aType /* = ParserError::Type::Parser */)
{
	return { aMessage, arLoc, aType };
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
 * @param akinds The token kinds to match.
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
 * @param aMessage The error message to use if consumption fails.
 * @return The consumed token.
 */
const Token& Parser::Consume(TokenKind akind, std::string_view aMessage) 
{
	if (Check(akind)) 
	{
		return Advance();
	}

	throw GenerateError(aMessage, Peek().Loc);
}