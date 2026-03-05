#include <iostream>
#include <string>
#include <vector>
#include "parser.h"
#include "io.h"

struct Test
{
    std::string expr;
    bool valid;
    int expected;
};

static bool runParserTests(IWriter& out)
{
    std::vector<Test> tests =
    {
        {"42", true, 42},
        {"-5", true, -5},
        {"1+3*4", true, 13},

        {"1+(3*4)", true, 13},
        {"-(2+(3+5)*2)", true, -18},

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

            std::string msg = "FAIL: " + t.expr + " (expected "
                + (t.valid ? "valid" : "invalid");

            if (t.valid)
            {
                msg += " result " + std::to_string(t.expected);
            }

            msg += ")";
            out.writeLine(msg);
        }
        else
        {
            out.writeLine("PASS: " + t.expr);
        }
    }

    return allPassed;
}

// Optional: a DI-focused test that proves runInteractive can be driven without console.
// This keeps it minimal and does not duplicate main.cpp.
static bool testInteractiveWithoutConsole()
{
    VectorReader in({ "1+2", "" });
    StringWriter out;

    while (true)
    {
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

    const auto& lines = out.lines();
    for (const auto& line : lines)
    {
        if (line == "Result: 3")
        {
            return true;
        }
    }

    return false;
}

int main()
{
    ConsoleWriter out;

    out.writeLine("Running parser tests...");
    bool parserPassed = runParserTests(out);
    out.writeLine(parserPassed ? "\nAll parser tests passed." : "\nSome parser tests failed.");

    out.writeLine("");
    out.writeLine("Running DI smoke test...");
    bool diPassed = testInteractiveWithoutConsole();
    out.writeLine(diPassed ? "DI smoke test passed." : "DI smoke test failed.");

    return (parserPassed && diPassed) ? 0 : 1;
}
