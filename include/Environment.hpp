#ifndef REPL_ENVIRONMENT_HPP
#define REPL_ENVIRONMENT_HPP

#include <variant>
#include <map>
#include <string_view>
#include "Token.hpp"
#include "Types.hpp"


using InterpreterValue = std::variant
	<
		std::monostate, 
		uint8_t, uint16_t, uint32_t, uint64_t,
		int8_t, int16_t, int32_t, int64_t,
		float, double,
		std::string, char,
		bool, 
		nullptr_t
	>;

struct Symbol
{
	Types::TypeKind Type{};
	InterpreterValue Value{};
	bool IsConst = false;
};

class Environment
{
public:
	InterpreterValue Get(const Token& arIdentifier);
	void Define(std::string_view aView, Types::TypeKind aType, InterpreterValue aValue);

private:
	std::map<std::string, Symbol> EnvironmentSymbols;
};

#endif // REPL_ENVIRONMENT_HPP