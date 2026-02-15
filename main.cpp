#include <iostream>
#include <string>
#include <vector>
#include "parser.h"

struct Test
{
    std::string expr;
    bool valid;
    int expected;
};

static bool runTests()
{
    std::vector<Test> tests =
    {
        {"42", true, 42},
        {"-5", true, -5},
        {"1+3*4", true, 16},
        {"1+(3*4)", true, 13},
        {"-(2+(3+5)*2)", true, -20},

        // Division by zero
        {"5/0", false, 0},

        // Double sign should be rejected
        {"--5", false, 0},
        {"+-5", false, 0},
        {"-+5", false, 0},

        // Overflow edge cases
        {"2147483647", true, 2147483647},     // INT_MAX
        {"2147483648", false, 0},             // INT_MAX + 1
        {"-2147483648", true, -2147483648},   // INT_MIN
        {"-2147483649", false, 0},            // INT_MIN - 1
        {"999999999999", false, 0},           // very large

        // Parentheses edge cases
        {"-(2147483648)", true, -2147483648},
        {"()", false, 0},
        {"(5", false, 0},
        {"5)", false, 0},

        // Invalid numbers
        {"abc", false, 0},
        {"abc42", false, 0},
        {"42abc", false, 0},

        // Whitespace handling
        {"   7   ", true, 7},
        {" (  -12  ) ", true, -12}
    };

    bool allPassed = true;

    for (const auto& t : tests)
    {
        Parser p(t.expr);
        int result = 0;
        bool ok = p.evaluate(result);

        bool passed = (ok == t.valid) && (!ok || result == t.expected);

        if (!passed)
        {
            allPassed = false;
            std::cout << "FAIL: " << t.expr << " (expected "
                      << (t.valid ? "valid" : "invalid");

            if (t.valid)
            {
                std::cout << " result " << t.expected;
            }

            std::cout << ")\n";
        }
        else
        {
            std::cout << "PASS: " << t.expr << '\n';
        }
    }

    return allPassed;
}

static void runInteractive()
{
    std::cout << "\nEnter expressions to evaluate.\n";
    std::cout << "Press Enter on an empty line to quit.\n\n";

    while (true)
    {
        std::cout << "Enter expression (press Enter to quit): ";

        std::string input;
        std::getline(std::cin, input);

        if (!std::cin || input.empty())
        {
            break;
        }

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
    }
}

int main()
{
    std::cout << "Running tests...\n";
    bool passed = runTests();

    std::cout << (passed ? "\nAll tests passed.\n" : "\nSome tests failed.\n");

    runInteractive();

    return passed ? 0 : 1;
}
