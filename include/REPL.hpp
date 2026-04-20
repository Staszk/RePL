#ifndef REPL_REPL_HPP
#define REPL_REPL_HPP

#include <string_view>

class REPL final
{
public:
	static void Run();

private:
	static void PrintHelp();
	static void ClearConsole();
	static void HandleInput(const std::string_view input);
};

#endif // REPL_REPL_HPP