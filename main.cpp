#include <iostream>
#include <string>
#include "parser.h"

int main()
{
    std::cout << "Enter expression: ";

    std::string input;
    std::getline(std::cin, input);

    Parser parser(input);

    int result = 0;
    if (parser.evaluate(result))
    {
        std::cout << "Result: " << result << '\n';
    }
    else
    {
        std::cout << "Invalid expression.\n";
    }

    return 0;
}
