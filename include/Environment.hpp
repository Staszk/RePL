#ifndef REPL_ENVIRONMENT_HPP
#define REPL_ENVIRONMENT_HPP

#include <variant>
#include <map>
#include <string_view>
#include "Token.hpp"
#include "Types.hpp"

using InterpreterValue = std::variant
	<
		opaque, 
		uint08, uint16, uint32, uint64,
		int8_t, int16_t, int32_t, int64_t,
		real32, real64,
		std::string, char08,
		binary, 
		nilptr
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
	void Define(std::string_view aView, Types::TypeKind aType, bool abIsConst, InterpreterValue aValue);

private:
	std::map<std::string, Symbol> EnvironmentSymbols;
};

#endif // REPL_ENVIRONMENT_HPP