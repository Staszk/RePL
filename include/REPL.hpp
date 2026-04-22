#ifndef REPL_REPL_HPP
#define REPL_REPL_HPP

#include <string_view>
#include "Interpreter.hpp"

class REPL final
{
public:
	static void Run();

private:
	static void PrintHelp();
	static void ClearConsole();
	static void HandleInput(const std::string_view input);

	static Interpreter _Interpreter;
};

#endif // REPL_REPL_HPP