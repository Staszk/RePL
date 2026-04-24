#include "Environment.hpp"
#include "Token.hpp"
#include <iostream>

InterpreterValue Environment::Get(const Token& arIdentifier)
{
	std::string key(arIdentifier.Value);
	if (EnvironmentVariables.contains(key))
	{
		return EnvironmentVariables.at(key);
	}

	return InterpreterValue(std::monostate{});
}

void Environment::Define(std::string_view aView, InterpreterValue aValue)
{
	EnvironmentVariables[std::string(aView)] = aValue;
}
