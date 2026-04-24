#ifndef REPL_ENVIRONMENT_HPP
#define REPL_ENVIRONMENT_HPP

#include <variant>
#include <unordered_map>
#include <string_view>
#include "Token.hpp"

using InterpreterValue = std::variant<std::monostate, int64_t, float, std::string, bool, nullptr_t>;

class Environment
{
public:
	InterpreterValue Get(const Token& arIdentifier);
	void Define(std::string_view aView, InterpreterValue aValue);

private:
	std::unordered_map<std::string, InterpreterValue> EnvironmentVariables;
};

#endif // REPL_ENVIRONMENT_HPP