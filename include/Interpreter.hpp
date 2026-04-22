#ifndef REPL_INTERPRETER_HPP
#define REPL_INTERPRETER_HPP

#include <string_view>
#include <variant>
#include <string>
#include <memory>
#include "Token.hpp"

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

using InterpreterValue = std::variant<std::monostate, int64_t, float, std::string, bool, nullptr_t>;

class InterpreterError : public std::exception
{
public:
	InterpreterError(const std::string_view aMessage)
	    : std::exception(), Message(aMessage){}
	
	const std::string_view Message;
};

class Interpreter
{
public:
	static constexpr auto Printer = overloaded 
	{
		[](std::monostate) { return std::string("null"); },
		[](int64_t intValue) { return std::to_string(intValue); },
		[](float floatValue) { return std::to_string(floatValue); },
		[](const std::string& stringValue) { return stringValue; },
		[](bool boolValue) { return std::string(boolValue ? "True" : "False"); },
		[](nullptr_t) { return std::string("null"); }
	};

	std::pair<InterpreterValue, bool> BeginInterpret(const std::unique_ptr<class ASTNode>& arNodePtr);
	InterpreterValue Interpret(const class ASTNode& node);
	InterpreterValue Interpret(const class ExprNode& node);
	InterpreterValue Interpret(const class KeywordLiteralExprNode& node);
	InterpreterValue Interpret(const class IntLiteralExprNode& node);
	InterpreterValue Interpret(const class FloatLiteralExprNode& node);
	InterpreterValue Interpret(const class StringLiteralExprNode& node);
	InterpreterValue Interpret(const class IdentifierExprNode& node);
	InterpreterValue Interpret(const class BinaryExprNode& node);
	InterpreterValue Interpret(const class UnaryExprNode& node);
	InterpreterValue Interpret(const class GroupingExprNode& node);

	static inline InterpreterError GenerateError(const std::string_view aMessage);
private:
	InterpreterValue PreviousResult{};
};

#endif // REPL_INTERPRETER_HPP