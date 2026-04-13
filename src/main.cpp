#include "Lexer.hpp"

#include <fstream>
#include <sstream>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        return 1;
    }

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

    return 0;
}