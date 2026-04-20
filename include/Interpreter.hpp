#ifndef REPL_INTERPRETER_HPP
#define REPL_INTERPRETER_HPP

#include <string_view>
#include <variant>
#include <string>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

using InterpreterValue = std::variant<std::monostate, int64_t, float, std::string, bool, nullptr_t>;

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
};

#endif // REPL_INTERPRETER_HPP