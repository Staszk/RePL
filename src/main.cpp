#include "Lexer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

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

        Lexer lexer(content.c_str());
        lexer.PrintContent();
        lexer.PrintTokens();

        std::puts("Done");
    }
    else
    {
        bool shouldExit = false;
        while (!shouldExit)
        {
            std::string input;
            std::cout << "> ";
            std::getline(std::cin, input);

            if (input == "exit")
            {
                shouldExit = true;
                continue;
            }

            Lexer lexer(input.c_str());
            lexer.PrintTokens();
        }
    }

    return 0;
}