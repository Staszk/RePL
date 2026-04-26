#include "Interpreter.hpp"
#include "AST.hpp"
#include "Token.hpp"
#include <charconv>
#include <concepts>
#include <system_error>
#include <format>
#include <iostream>

namespace 
{
	template<typename T>
	concept Numeric = std::is_arithmetic_v<T> && !std::is_same_v<T, bool>;

	static constexpr auto Printer = overloaded 
	{
		[](std::monostate) { return std::string("null"); },
		[]<Numeric T>(T numericValue) { return std::to_string(numericValue); },
		[](const std::string& stringValue) { return stringValue; },
		[](bool boolValue) { return std::string(boolValue ? "True" : "False"); },
		[](nullptr_t) { return std::string("null"); },
		[](auto) { return std::string("not implemented"); }
	};

	static constexpr auto PlusOp = overloaded
	{
		[](std::monostate, std::monostate) { return InterpreterValue(std::monostate{}); },
		[]<Numeric T, Numeric U>(T left, U right) { return InterpreterValue(left + right); },
		[](const std::string& left, const std::string& right) { return InterpreterValue(std::string(left + right)); },
		[](auto, auto) { return InterpreterValue(std::monostate{}); }
	};

	static constexpr auto MinusOp = overloaded
	{
		[](std::monostate, std::monostate) { return InterpreterValue(std::monostate{}); },
		[]<Numeric T, Numeric U>(T left, U right) { return InterpreterValue(left - right); },
		[](const std::string& left, const std::string& right) 
		{
			std::string result(left);
			size_t pos = 0;
    		if ((pos = result.find(right, pos)) != std::string::npos) 
			{
        		result.erase(pos, right.length());
    		};

			return InterpreterValue(result);
		},
		[](auto, auto) { return InterpreterValue(std::monostate{}); }
	};

	static constexpr auto MultiplyOp = overloaded
	{
		[](std::monostate, std::monostate) { return InterpreterValue(std::monostate{}); },
		[]<Numeric T, Numeric U>(T left, U right) { return InterpreterValue(left * right); },
		[](const std::string&, const std::string&) { return InterpreterValue(std::monostate{}); },
		[]<Numeric T>(T left, const std::string& right)
		{
			if (left < 0.0) throw Interpreter::GenerateError("Cannot negatively repeat a string.");
			if (left == 1.0) return InterpreterValue(right);

			std::string result;
			size_t iterations = static_cast<size_t>(left);
			result.reserve(right.length() * iterations);
			for (size_t i{0}; i < iterations; ++i) 
			{
				result += right;
			}
			return InterpreterValue(result);
		},
		[]<Numeric T>(const std::string& left, T right)
		{
			if (right < 0.0) throw Interpreter::GenerateError("Cannot negatively repeat a string.");
			if (right == 1.0) return InterpreterValue(left);

			std::string result;
			size_t iterations = static_cast<size_t>(right);
			result.reserve(left.length() * iterations);
			for (size_t i{0}; i < iterations; ++i) 
			{
				result += left;
			}
			return InterpreterValue(result);
		},
		[](auto, auto) { return InterpreterValue(std::monostate{}); }
	};

	static constexpr auto DivideOp = overloaded
	{
		[](std::monostate, std::monostate) { return InterpreterValue(std::monostate{}); },
		[]<Numeric T, Numeric U>(T left, U right) { return InterpreterValue(left / right); },
		[](const std::string& left, const std::string& right) 
		{
			std::string result(left);
			size_t pos = 0;
    		while ((pos = result.find(right, pos)) != std::string::npos) 
			{
        		result.erase(pos, right.length());
    		};

			return InterpreterValue(result);
		},
		[](auto, auto) { return InterpreterValue(std::monostate{}); }
	};

	static constexpr auto GreaterOp = overloaded
	{
		[](std::monostate, std::monostate) { return false; },
		[]<Numeric T, Numeric U>(T left, U right) { return left > right; },
		[](const std::string& left, const std::string& right) { return left > right; },
		[](auto, auto) { return false; }
	};

	static constexpr auto GreaterEqualOp = overloaded
	{
		[](std::monostate, std::monostate) { return false; },
		[]<Numeric T, Numeric U>(T left, U right) { return left >= right; },
		[](const std::string& left, const std::string& right) { return left >= right; },
		[](auto, auto) { return false; }
	};

	static constexpr auto LesserOp = overloaded
	{
		[](std::monostate, std::monostate) { return false; },
		[]<Numeric T, Numeric U>(T left, U right) { return left < right; },
		[](const std::string& left, const std::string& right) { return left < right; },
		[](auto, auto) { return false; }
	};

	static constexpr auto LesserEqualOp = overloaded
	{
		[](std::monostate, std::monostate) { return false; },
		[]<Numeric T, Numeric U>(T left, U right) { return left <= right; },
		[](const std::string& left, const std::string& right) { return left <= right; },
		[](auto, auto) { return false; }
	};

	static constexpr auto EqualEqualOp = overloaded
	{
		[](std::monostate, std::monostate) { return true; },
		[]<typename T>(const T& left, const T& right) { return left == right; },
		[]<Numeric T, Numeric U>(T left, U right) { return left == right; },
		[]<Numeric T>(T left, bool right) { return static_cast<bool>(left) == right; },
		[]<Numeric T>(bool left, T right) { return left == static_cast<bool>(right); },
		[](bool left, const std::string& right) 
		{
			if (right == "true") return left == true;
			if (right == "false") return left == false;
			return false;
		},
		[](const std::string& left, bool right) 
		{
			if (left == "true") return right == true;
			if (left == "false") return right == false;
			return false;
		},
		[]<Numeric T>(const std::string& left, T right) 
		{
			T value;
			std::errc ec{};
			if (left == "true") value = T{1};
			else if (left == "false") value = T{0};
			else
			{
				auto [ptr, ec] = std::from_chars(left.data(), left.data() + left.size(), value);
			}
			return ec == std::errc() ? value == right : true;
		},
		[]<Numeric T>(T left, const std::string& right) 
		{
			T value;
			std::errc ec{};
			if (right == "true") value = T{1};
			else if (right == "false") value = T{0};
			else
			{
				auto [ptr, ec] = std::from_chars(right.data(), right.data() + right.size(), value);
			}
			return ec == std::errc() ? value == left : true;
		},
		[]<typename T, typename U>(const T& left, const U& right) requires (!Numeric<T> || !Numeric<U>) { return false; }
	};

	static constexpr auto NotEqualOp = overloaded
	{
		[](std::monostate, std::monostate) { return false; },
		[]<typename T>(const T& left, const T& right) { return left != right; },
		[]<Numeric T, Numeric U>(T left, U right) -> bool { return left != right; },
		[](bool left, const std::string& right) 
		{
			if (right == "true") return left != true;
			if (right == "false") return left != false;
			return false;
		},
		[](const std::string& left, bool right) 
		{
			if (left == "true") return right != true;
			if (left == "false") return right != false;
			return false;
		},
		[]<Numeric T>(const std::string& left, T right) 
		{
			T value;
			std::errc ec{};
			if (left == "true") value = T{1};
			else if (left == "false") value = T{0};
			else
			{
				auto [ptr, ec] = std::from_chars(left.data(), left.data() + left.size(), value);
			}
			return ec == std::errc() ? value != right : true;
		},
		[]<Numeric T>(T left, const std::string& right) 
		{
			T value;
			std::errc ec{};
			if (right == "true") value = T{1};
			else if (right == "false") value = T{0};
			else
			{
				auto [ptr, ec] = std::from_chars(right.data(), right.data() + right.size(), value);
			}
			return ec == std::errc() ? value != left : true;
		},
		[]<typename T, typename U>(const T&, const U&) requires (!Numeric<T> || !Numeric<U>) { return true; }
	};

	static constexpr auto NegateOp = overloaded
	{
		[](std::monostate) { return InterpreterValue(std::monostate{}); },
		[]<Numeric T>(T numericValue ) { return InterpreterValue(-numericValue); },
		[](const std::string&) { return InterpreterValue(std::monostate{}); },
		[](bool boolValue) { return InterpreterValue(-static_cast<int8_t>(boolValue)); },
		[](nullptr_t) { return InterpreterValue(std::monostate{}); }
	};

	static constexpr auto TruthOp = overloaded
	{
		[](std::monostate) { return false; },
		[]<Numeric T>(T numericValue) { return numericValue != static_cast<T>(0); },
		[](const std::string& stringValue) { return !stringValue.empty(); },
		[](bool boolValue) { return boolValue; },
		[](nullptr_t) { return false; }
	};
}

std::string Interpreter::PrintValue(InterpreterValue aVal)
{
	return std::visit(Printer, aVal);
}

InterpreterValue Interpreter::Interpret(const ExprNode &node)
{
	return InterpreterValue();
}

InterpreterValue Interpreter::Interpret(const KeywordLiteralExprNode &node)
{
	if (node.ValueToken.Value == "_")
	{
		return PreviousResult;
	}
	return std::string(node.ValueToken.Value);
}

InterpreterValue Interpreter::Interpret(const IntLiteralExprNode &node)
{
	int64_t intValue;
	std::from_chars(node.ValueToken.Value.data(), node.ValueToken.Value.data() + node.ValueToken.Value.size(), intValue);
	return intValue;
}

InterpreterValue Interpreter::Interpret(const FloatLiteralExprNode &node)
{
	float floatValue;
	std::from_chars(node.ValueToken.Value.data(), node.ValueToken.Value.data() + node.ValueToken.Value.size(), floatValue);
	return floatValue;
}

InterpreterValue Interpreter::Interpret(const StringLiteralExprNode &node)
{
	std::string stringValue(node.ValueToken.Value);
	// Remove the surrounding quotes from the string literal
	if (!stringValue.empty() && stringValue.front() == '\"' && stringValue.back() == '\"')
	{
		stringValue = stringValue.substr(1, stringValue.size() - 2);
	}
	return stringValue;
}

InterpreterValue Interpreter::Interpret(const IdentifierExprNode &node)
{
	InterpreterValue value = Env.Get(node.ValueToken);
	return Env.Get(node.ValueToken);
}

InterpreterValue Interpreter::Interpret(const BinaryExprNode &node)
{
	const InterpreterValue leftValue = node.Left->Accept(*this);
	const InterpreterValue rightValue = node.Right->Accept(*this);

	switch (node.OperatorToken.Kind)
	{
		using enum TokenKind;
	case Plus:
		return std::visit(PlusOp, leftValue, rightValue);
	case Minus:
		return std::visit(MinusOp, leftValue, rightValue);
	case Asterisk:
		return std::visit(MultiplyOp, leftValue, rightValue);
	case Slash:
		return std::visit(DivideOp, leftValue, rightValue);
	case Greater:
		return std::visit(GreaterOp, leftValue, rightValue);
	case GreaterEqual:
		return std::visit(GreaterEqualOp, leftValue, rightValue);
	case Lesser:
		return std::visit(LesserOp, leftValue, rightValue);
	case LesserEqual:
		return std::visit(LesserEqualOp, leftValue, rightValue);
	case EqualEqual:
		return std::visit(EqualEqualOp, leftValue, rightValue);
	case NotEqual:
		return std::visit(NotEqualOp, leftValue, rightValue);
		break;
	}

	return InterpreterValue();
}

InterpreterValue Interpreter::Interpret(const UnaryExprNode &node)
{
	const InterpreterValue operandValue = node.Operand->Accept(*this);

	switch (node.OperatorToken.Kind)
	{
		using enum TokenKind;
	case Minus:
		return std::visit(NegateOp, operandValue);
	case Bang:
		return !std::visit(TruthOp, operandValue);
	}

	return InterpreterValue();
}

InterpreterValue Interpreter::Interpret(const GroupingExprNode &node)
{
	return node.Inner->Accept(*this);
}

void Interpreter::Execute(const std::unique_ptr<StmntNode>& arNodePtr)
{
	arNodePtr->Accept(*this);
}

void Interpreter::Execute(const VarDeclStmntNode &node)
{
	InterpreterValue value{};
	if (node.Expression)
	{
		value = node.Expression->Accept(*this);
	}

	Env.Define(node.IdentifierToken.Value, static_cast<Types::TypeKind>(node.Specifier), value);
}

InterpreterError Interpreter::GenerateError(const std::string_view aMessage)
{
	return { aMessage };
}
