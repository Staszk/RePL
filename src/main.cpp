#include "REPL.hpp"
#include "Compiler.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

// Testing
#define TEST_LEXER "test" \
    "\n" \
    "int main() {\n" \
    "    // This is a line comment\n" \
    "    /* This is a block comment */\n" \
    "    int x = 42;\n" \
    "    string s = \"Hello, World!\";\n" \
    "}\n"

/**
 * @brief Program entry point for the RePL interpreter.
 *
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Exit status code.
 */
int main(int argc, char* argv[])
{
    const bool fileMode = argc > 1;

    if (fileMode)
    {
        std::string content{};
        std::ifstream file(argv[1]);
        if (file.is_open())
        {
            std::stringstream buffer;
            buffer << file.rdbuf();
            content = buffer.str();
            file.close();
        }

        Compiler::Compile(content);
    }
    else
    {
        REPL::Run();
    }

    return 0;
}