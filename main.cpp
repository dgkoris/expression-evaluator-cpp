#include <string>
#include "parser.h"
#include "io.h"

int main()
{
    ConsoleWriter out;
    ConsoleReader in;

    out.writeLine("");
    out.writeLine("Enter expressions to evaluate.");
    out.writeLine("Press Enter on an empty line to quit.");
    out.writeLine("");

    while (true)
    {
        out.writeLine("Enter expression (press Enter to quit): ");

        std::string input;
        if (!in.readLine(input) || input.empty())
        {
            break;
        }

        Parser parser(input);

        int result = 0;
        if (parser.evaluate(result))
        {
            out.writeLine("Result: " + std::to_string(result));
        }
        else
        {
            out.writeLine("Invalid expression.");
        }
    }

    return 0;
}
