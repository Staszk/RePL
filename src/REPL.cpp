#include "REPL.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include <iostream>

Interpreter REPL::_Interpreter{};

namespace
{
	bool MatchesAny(const std::string_view input, const std::initializer_list<std::string_view> options)
	{
		for (const auto& option : options)
		{
			if (input == option)
			{
				return true;
			}
		}
		return false;
	}

	void SetScrollingRegion(int top, int bottom) 
	{
		// \033[top;bottomr
		std::cout << "\033[" << top << ";" << bottom << "r" << std::flush;
	}

	void MoveTo(int row, int col) 
	{
		// \033[row;colH
		std::cout << "\033[" << row << ";" << col << "H" << std::flush;
	}

	void Setup()
	{
		// Clear screen first
		std::cout << "\033[2J" << std::flush;

		MoveTo(1, 1);
		std::cout << "\033[44;37m" << " RePL (Recreational Programming Language) REPL (Read - Evaluate - Print - Loop) " << "\033[0m";

		SetScrollingRegion(2, 99);
		MoveTo(2, 1);
	}
}

void REPL::Run()
{
	// 1. Enter the Alternate Screen Buffer
	// \033[?1049h is the ANSI escape code to switch buffers
	std::cout << "\033[?1049h" << std::flush;

	Setup();

	bool shouldExit = false;
	while (!shouldExit)
	{
		std::string input;
		std::cout << "> ";
		std::getline(std::cin, input);

		if (input.empty())
		{
			continue;
		}
		
		if (MatchesAny(input, {":h", ":help", ":?"}))
		{
			PrintHelp();
		}
		else if (MatchesAny(input, {":clear", ":cls", ":c"}))
		{
			ClearConsole();
		}
		else if (MatchesAny(input, {":exit", ":quit", ":q"}))
		{
			shouldExit = true;
		}
		else
		{
			HandleInput(input);
		}
	}

	SetScrollingRegion(1, 99);
	MoveTo(99, 1);

	std::cout << "\033[?1049l" << std::flush;
}

void REPL::PrintHelp()
{
	std::cout << "RePL Help:\n"
			  << "  :h, :help, :?       -> Show this help message\n"
			  << "  :clear              -> Clear the console\n"
			  << "  :q, :quit, :exit    -> Exit the RePL REPL\n";
}

void REPL::ClearConsole()
{
	Setup();
}

void REPL::HandleInput( std::string_view input)
{
	Lexer lexer(input, false);
	Parser parser(lexer.tokens());
	const std::unique_ptr<ASTNode>& root = parser.GetRoot();
	if (root != nullptr)
	{
		auto [value, success] = _Interpreter.BeginInterpret(root);
		if (success)
		{
			std::cout << std::visit(Interpreter::Printer, value) << '\n';
		}
	}
}