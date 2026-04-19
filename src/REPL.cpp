#include "REPL.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Interpreter.hpp"
#include <iostream>

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
}

void REPL::Run()
{
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
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void REPL::HandleInput( std::string_view input)
{
    Lexer lexer(input, false);
    Parser parser(lexer.tokens());
    Interpreter interpreter;
    InterpreterValue result = parser.GetRoot()->Accept(interpreter);

    std::cout << std::visit(Interpreter::Printer, result) << '\n';
}