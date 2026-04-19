#include "Interpreter.hpp"
#include "AST.hpp"
#include "Token.hpp"
#include <charconv>

namespace 
{
    static constexpr auto PlusOp = overloaded
    {
        [](std::monostate, std::monostate) { return InterpreterValue(std::monostate{}); },
        [](int64_t left, int64_t right) { return InterpreterValue(left + right); },
        [](int64_t left, float right) { return InterpreterValue(static_cast<float>(left) + right); },
        [](float left, int64_t right) { return InterpreterValue(left + static_cast<float>(right)); },
        [](float left, float right) { return InterpreterValue(left + right); },
        [](const std::string& left, const std::string& right) { return InterpreterValue(std::string(left + right)); },
        [](auto, auto) { return InterpreterValue(std::monostate{}); }
    };

    static constexpr auto MinusOp = overloaded
    {
        [](std::monostate, std::monostate) { return InterpreterValue(std::monostate{}); },
        [](int64_t left, int64_t right) { return InterpreterValue(left - right); },
        [](int64_t left, float right) { return InterpreterValue(static_cast<float>(left) - right); },
        [](float left, int64_t right) { return InterpreterValue(left - static_cast<float>(right)); },
        [](float left, float right) { return InterpreterValue(left - right); },
        [](const std::string&, const std::string&) { return InterpreterValue(std::monostate{}); },
        [](auto, auto) { return InterpreterValue(std::monostate{}); }
    };

    static constexpr auto MultiplyOp = overloaded
    {
        [](std::monostate, std::monostate) { return InterpreterValue(std::monostate{}); },
        [](int64_t left, int64_t right) { return InterpreterValue(left * right); },
        [](int64_t left, float right) { return InterpreterValue(static_cast<float>(left) * right); },
        [](float left, int64_t right) { return InterpreterValue(left * static_cast<float>(right)); },
        [](float left, float right) { return InterpreterValue(left * right); },
        [](const std::string&, const std::string&) { return InterpreterValue(std::monostate{}); },
        [](auto, auto) { return InterpreterValue(std::monostate{}); }
    };

    static constexpr auto DivideOp = overloaded
    {
        [](std::monostate, std::monostate) { return InterpreterValue(std::monostate{}); },
        [](int64_t left, int64_t right) { return right != 0 ? InterpreterValue(left / right) : InterpreterValue(std::monostate{}); },
        [](int64_t left, float right) { return right != 0.0 ? InterpreterValue(static_cast<float>(left) / right) : InterpreterValue(std::monostate{}); },
        [](float left, int64_t right) { return right != 0 ? InterpreterValue(left / static_cast<float>(right)) : InterpreterValue(std::monostate{}); },
        [](float left, float right) { return right != 0.0 ? InterpreterValue(left / right) : InterpreterValue(std::monostate{}); },
        [](const std::string&, const std::string&) { return InterpreterValue(std::monostate{}); },
        [](auto, auto) { return InterpreterValue(std::monostate{}); }
    };

    static constexpr auto NegateOp = overloaded
    {
        [](std::monostate) { return InterpreterValue(std::monostate{}); },
        [](int64_t intValue) { return InterpreterValue(-intValue); },
        [](float floatValue) { return InterpreterValue(-floatValue); },
        [](const std::string&) { return InterpreterValue(std::monostate{}); },
        [](nullptr_t) { return InterpreterValue(std::monostate{}); }
    };

    static constexpr auto TruthOp = overloaded
    {
        [](std::monostate) { return false; },
        [](int64_t intValue) { return intValue != 0; },
        [](float floatValue) { return floatValue != 0.0; },
        [](const std::string& stringValue) { return !stringValue.empty(); },
        [](nullptr_t) { return false; }
    };
}

InterpreterValue Interpreter::Interpret(const ASTNode &node)
{
    return InterpreterValue();
}

InterpreterValue Interpreter::Interpret(const ExprNode &node)
{
    return InterpreterValue();
}

InterpreterValue Interpreter::Interpret(const KeywordLiteralExprNode &node)
{
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
    return std::string(node.ValueToken.Value);
}

InterpreterValue Interpreter::Interpret(const BinaryExprNode &node)
{
    InterpreterValue leftValue = node.Left->Accept(*this);
    InterpreterValue rightValue = node.Right->Accept(*this);

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
    }

    return InterpreterValue();

}

InterpreterValue Interpreter::Interpret(const UnaryExprNode &node)
{
    InterpreterValue operandValue = node.Operand->Accept(*this);

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
