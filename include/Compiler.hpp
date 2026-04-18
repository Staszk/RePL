#ifndef REPL_COMPILER_HPP
#define REPL_COMPILER_HPP

#include "Lexer.hpp"
#include "Parser.hpp"

class Compiler final
{
public:
    static void Compile(const std::string_view source)
    {
        Lexer lexer(source);
        Parser parser(lexer.tokens());
    }
};

#endif // REPL_COMPILER_HPP