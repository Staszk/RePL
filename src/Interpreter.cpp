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
		[](opaque) { return string("null"); },
		[](char08 charValue) { return string{charValue}; },
		[]<Numeric T>(T numericValue) { return std::to_string(numericValue); },
		[](const string& stringValue) { return stringValue; },
		[](bool boolValue) { return string(boolValue ? "True" : "False"); },
		[](nullptr_t) { return string("null"); },
		[](auto) { return string("not implemented"); }
	};

	static constexpr auto PlusOp = overloaded
	{
		[](opaque, opaque) { return InterpreterValue(opaque{}); },
		[]<Numeric T, Numeric U>(T left, U right) { return InterpreterValue(left + right); },
		[](const string& left, const string& right) { return InterpreterValue(string(left + right)); },
		[](auto, auto) { return InterpreterValue(opaque{}); }
	};

	static constexpr auto MinusOp = overloaded
	{
		[](opaque, opaque) { return InterpreterValue(opaque{}); },
		[]<Numeric T, Numeric U>(T left, U right) { return InterpreterValue(left - right); },
		[](const string& left, const string& right) 
		{
			string result(left);
			size_t pos = 0;
    		if ((pos = result.find(right, pos)) != string::npos) 
			{
        		result.erase(pos, right.length());
    		};

			return InterpreterValue(result);
		},
		[](auto, auto) { return InterpreterValue(opaque{}); }
	};

	static constexpr auto MultiplyOp = overloaded
	{
		[](opaque, opaque) { return InterpreterValue(opaque{}); },
		[]<Numeric T, Numeric U>(T left, U right) { return InterpreterValue(left * right); },
		[](const string&, const string&) { return InterpreterValue(opaque{}); },
		[]<Numeric T>(T left, const string& right)
		{
			if (left < 0.0) throw Interpreter::GenerateError("Cannot negatively repeat a string.");
			if (left == 1.0) return InterpreterValue(right);

			string result;
			size_t iterations = static_cast<size_t>(left);
			result.reserve(right.length() * iterations);
			for (size_t i{0}; i < iterations; ++i) 
			{
				result += right;
			}
			return InterpreterValue(result);
		},
		[]<Numeric T>(const string& left, T right)
		{
			if (right < 0.0) throw Interpreter::GenerateError("Cannot negatively repeat a string.");
			if (right == 1.0) return InterpreterValue(left);

			string result;
			size_t iterations = static_cast<size_t>(right);
			result.reserve(left.length() * iterations);
			for (size_t i{0}; i < iterations; ++i) 
			{
				result += left;
			}
			return InterpreterValue(result);
		},
		[](auto, auto) { return InterpreterValue(opaque{}); }
	};

	static constexpr auto DivideOp = overloaded
	{
		[](opaque, opaque) { return InterpreterValue(opaque{}); },
		[]<Numeric T, Numeric U>(T left, U right) { return InterpreterValue(left / right); },
		[](const string& left, const string& right) 
		{
			string result(left);
			size_t pos = 0;
    		while ((pos = result.find(right, pos)) != string::npos) 
			{
        		result.erase(pos, right.length());
    		};

			return InterpreterValue(result);
		},
		[](auto, auto) { return InterpreterValue(opaque{}); }
	};

	static constexpr auto GreaterOp = overloaded
	{
		[](opaque, opaque) { return false; },
		[]<Numeric T, Numeric U>(T left, U right) { return left > right; },
		[](const string& left, const string& right) { return left > right; },
		[](auto, auto) { return false; }
	};

	static constexpr auto GreaterEqualOp = overloaded
	{
		[](opaque, opaque) { return false; },
		[]<Numeric T, Numeric U>(T left, U right) { return left >= right; },
		[](const string& left, const string& right) { return left >= right; },
		[](auto, auto) { return false; }
	};

	static constexpr auto LesserOp = overloaded
	{
		[](opaque, opaque) { return false; },
		[]<Numeric T, Numeric U>(T left, U right) { return left < right; },
		[](const string& left, const string& right) { return left < right; },
		[](auto, auto) { return false; }
	};

	static constexpr auto LesserEqualOp = overloaded
	{
		[](opaque, opaque) { return false; },
		[]<Numeric T, Numeric U>(T left, U right) { return left <= right; },
		[](const string& left, const string& right) { return left <= right; },
		[](auto, auto) { return false; }
	};

	static constexpr auto EqualEqualOp = overloaded
	{
		[](opaque, opaque) { return true; },
		[]<typename T>(const T& left, const T& right) { return left == right; },
		[]<Numeric T, Numeric U>(T left, U right) { return left == right; },
		[]<Numeric T>(T left, bool right) { return static_cast<bool>(left) == right; },
		[]<Numeric T>(bool left, T right) { return left == static_cast<bool>(right); },
		[](bool left, const string& right) 
		{
			if (right == "true") return left == true;
			if (right == "false") return left == false;
			return false;
		},
		[](const string& left, bool right) 
		{
			if (left == "true") return right == true;
			if (left == "false") return right == false;
			return false;
		},
		[]<Numeric T>(const string& left, T right) 
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
		[]<Numeric T>(T left, const string& right) 
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
		[](opaque, opaque) { return false; },
		[]<typename T>(const T& left, const T& right) { return left != right; },
		[]<Numeric T, Numeric U>(T left, U right) -> bool { return left != right; },
		[](bool left, const string& right) 
		{
			if (right == "true") return left != true;
			if (right == "false") return left != false;
			return false;
		},
		[](const string& left, bool right) 
		{
			if (left == "true") return right != true;
			if (left == "false") return right != false;
			return false;
		},
		[]<Numeric T>(const string& left, T right) 
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
		[]<Numeric T>(T left, const string& right) 
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
		[](opaque) { return InterpreterValue(opaque{}); },
		[]<Numeric T>(T numericValue ) { return InterpreterValue(-numericValue); },
		[](const string&) { return InterpreterValue(opaque{}); },
		[](bool boolValue) { return InterpreterValue(-static_cast<int8_t>(boolValue)); },
		[](nullptr_t) { return InterpreterValue(opaque{}); }
	};

	static constexpr auto TruthOp = overloaded
	{
		[](opaque) { return false; },
		[]<Numeric T>(T numericValue) { return numericValue != static_cast<T>(0); },
		[](const string& stringValue) { return !stringValue.empty(); },
		[](bool boolValue) { return boolValue; },
		[](nullptr_t) { return false; }
	};

	// Get Inferred Type
	static constexpr auto InferTypeOp = overloaded 
	{
        [](opaque) 			{ return Types::TypeKind::Opaque; },
        [](uint08) 			{ return Types::TypeKind::Uint08; },
        [](uint16) 			{ return Types::TypeKind::Uint16; },
        [](uint32) 			{ return Types::TypeKind::Uint32; },
        [](uint64) 			{ return Types::TypeKind::Uint64; },
        [](sint08) 			{ return Types::TypeKind::Sint08; },
        [](sint16) 			{ return Types::TypeKind::Sint16; },
        [](sint32) 			{ return Types::TypeKind::Sint32; },
        [](sint64) 			{ return Types::TypeKind::Sint64; },
        [](real32) 			{ return Types::TypeKind::Real32; },
        [](real64) 			{ return Types::TypeKind::Real64; },
        [](const string&) 	{ return Types::TypeKind::String; },
        [](char08) 			{ return Types::TypeKind::Char08; },
        [](binary) 			{ return Types::TypeKind::Opaque; },
		[](nilptr) 			{ return Types::TypeKind::NilPtr; },

        [](auto) { std::cout << "What?\n"; return Types::TypeKind::Opaque; }
    };

}

string Interpreter::PrintValue(InterpreterValue aVal)
{
	return std::visit(Printer, aVal);
}

InterpreterValue Interpreter::Interpret(const ExprNode &node)
{
	return InterpreterValue();
}

InterpreterValue Interpreter::Interpret(const KeywordLiteralExprNode &node)
{
	if (node.ValueToken.TypeOrKeyword == std::to_underlying(Keywords::KeywordKind::LastResult))
	{
		return PreviousResult;
	}
	return string(node.ValueToken.Value);
}

InterpreterValue Interpreter::Interpret(const IntLiteralExprNode &node)
{
	sint64 intValue;
	std::from_chars(node.ValueToken.Value.data(), node.ValueToken.Value.data() + node.ValueToken.Value.size(), intValue);
	return intValue;
}

InterpreterValue Interpreter::Interpret(const FloatLiteralExprNode &node)
{
	real32 floatValue;
	std::from_chars(node.ValueToken.Value.data(), node.ValueToken.Value.data() + node.ValueToken.Value.size(), floatValue);
	return floatValue;
}

InterpreterValue Interpreter::Interpret(const StringLiteralExprNode &node)
{
	string stringValue(node.ValueToken.Value);
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
	Types::TypeKind inferredType = static_cast<Types::TypeKind>(node.Specifier);
	if (node.Expression)
	{
		value = node.Expression->Accept(*this);

		if (inferredType == Types::TypeKind::Unspecified)
		{
			inferredType = std::visit(InferTypeOp, value);
		}
	}

	Env.Define(node.IdentifierToken.Value, inferredType, node.IsConst, value);
}

InterpreterError Interpreter::GenerateError(const std::string_view aMessage)
{
	return { aMessage };
}
