#ifndef REPL_PARSER_HPP
#define REPL_PARSER_HPP

#include "AST.hpp"
#include "Lexer.hpp"
#include <memory>
#include <vector>

class ParserError : public std::exception
{
public:
	enum class Type : uint8_t { Parser, Syntax };
	
	ParserError(const std::string_view aMessage, const TokenLocation& aLoc, Type aType = Type::Parser)
	    : std::exception(), Message(aMessage), Loc(aLoc), ErrorType(aType) {}
	
	const std::string_view Message;
	const TokenLocation& Loc;
	const Type ErrorType;
};

/**
 * @brief Recursive descent parser for the RePL language.
 */
class Parser {
public:
	Parser(const std::vector<Token>& tokens);

	/**
	 * @brief Get the parser root AST node.
	 *
	 * @return A reference to the root AST node pointer.
	 */
	std::unique_ptr<ExprNode>& GetRoot() { return Root; }
	
private:
	void Parse();
	void Synchronize();
	void RequireWhitespace(const std::string_view aMessage, std::ptrdiff_t aPreviousOffset = -1, std::ptrdiff_t aCurrentOffset = 0);
	ParserError GenerateError(const std::string_view aMessage, const TokenLocation& arLoc, ParserError::Type aType = ParserError::Type::Parser);
	const Token& Advance();
	const Token& Consume(TokenKind type, std::string_view message);
	const Token& Peek(std::ptrdiff_t aOffset = 0) const;
	const Token& RetrieveBinaryOperator();
	bool Check(TokenKind type) const;
	bool Match(std::initializer_list<TokenKind> aKinds);

	std::unique_ptr<ExprNode> ParseExpression();
	std::unique_ptr<ExprNode> ParseEquality();
	std::unique_ptr<ExprNode> ParseComparison();
	std::unique_ptr<ExprNode> ParseTerm();
	std::unique_ptr<ExprNode> ParseFactor();
	std::unique_ptr<ExprNode> ParseUnary();
	std::unique_ptr<ExprNode> ParsePrimary();

	std::unique_ptr<StmntNode> ParseStatement();
	std::unique_ptr<StmntNode> ParseExprStmnt();

	/**
	 * @brief Check if the current token position is valid for parsing.
	 * 
	 * @return True if the cursor is within the token stream bounds.
	 */
	bool CursorValid() const { return Cursor < TokenCount && Tokens[Cursor].Kind != TokenKind::End; }

	// Input
	const std::vector<Token>& Tokens;
	// State
	const size_t TokenCount{0};
	size_t Cursor{0};
	// Output
	std::unique_ptr<ExprNode> Root{};
	std::vector<std::unique_ptr<StmntNode>> Statements;
};

#endif // REPL_PARSER_HPP