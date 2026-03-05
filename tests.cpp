#include <iostream>
#include <string>
#include <vector>
#include <random>
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

// Generate a small random expression. Keeps it simple so it’s stable and fast.
static std::string randomExpr(std::mt19937& rng)
{
    std::uniform_int_distribution<int> digit(0, 9);
    std::uniform_int_distribution<int> op(0, 3);
    std::uniform_int_distribution<int> coin(0, 9);

    const char ops[] = { '+', '-', '*', '/' };

    auto randDigit = [&]() -> char { return static_cast<char>('0' + digit(rng)); };

    std::string expr;

    // Sometimes start with unary sign
    if (coin(rng) < 2)
    {
        expr += (coin(rng) < 5) ? '-' : '+';
    }

    // Start with either a digit or a parenthesised digit
    if (coin(rng) < 3)
    {
        expr += '(';
        expr += randDigit();
        expr += ')';
    }
    else
    {
        expr += randDigit();
    }

    // Add a few operator/value pairs
    const int terms = 1 + (coin(rng) % 6); // 1..6
    for (int i = 0; i < terms; i++)
    {
        expr += ops[op(rng)];

        // Sometimes add parentheses around the value
        if (coin(rng) < 3)
        {
            expr += '(';
            if (coin(rng) < 2)
            {
                expr += '-';
            }
            expr += randDigit();
            expr += ')';
        }
        else
        {
            if (coin(rng) < 2)
            {
                expr += '-';
            }
            expr += randDigit();
        }
    }

    return expr;
}

static bool fuzzParserDoesNotCrash()
{
    std::mt19937 rng(12345); // fixed seed so failures are reproducible

    const int iterations = 5000;
    int validCount = 0;
    int invalidCount = 0;

    for (int i = 0; i < iterations; i++)
    {
        std::string expr = randomExpr(rng);

        Parser p(expr);

        int result = 0;
        bool ok1 = p.evaluate(result);

        // Determinism check: running again should give same outcome
        int result2 = 0;
        bool ok2 = p.evaluate(result2);

        if (ok1 != ok2)
        {
            std::cout << "FUZZ FAIL (non-deterministic): " << expr << "\n";
            return false;
        }

        if (ok1 && result != result2)
        {
            std::cout << "FUZZ FAIL (different results): " << expr << "\n";
            return false;
        }

        if (ok1)
            validCount++;
        else
            invalidCount++;
    }

    std::cout << "Fuzz summary:\n";
    std::cout << "  Total expressions: " << iterations << "\n";
    std::cout << "  Valid results:     " << validCount << "\n";
    std::cout << "  Invalid results:   " << invalidCount << "\n";

    return true;
}

// Optional: a DI-focused test that proves runInteractive can be driven without console.
// This keeps it minimal and does not duplicate main.cpp.
static bool testInteractiveWithoutConsole()
{
    VectorReader in({ "1+2", "--2", "abc", "" });
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

    std::vector<std::string> expected =
    {
        "Result: 3",
        "Invalid expression.",
        "Invalid expression."
    };

    if (lines.size() != expected.size())
        return false;

    for (size_t i = 0; i < expected.size(); ++i)
    {
        if (lines[i] != expected[i])
            return false;
    }

    return true;
}

int main()
{
    ConsoleWriter out;

    out.writeLine("Running parser tests...");
    bool parserPassed = runParserTests(out);
    out.writeLine(parserPassed ? "\nAll parser tests passed." : "\nSome parser tests failed.");

    out.writeLine("");
    out.writeLine("Running fuzz tests...");
    bool fuzzPassed = fuzzParserDoesNotCrash();
    out.writeLine(fuzzPassed ? "Fuzz tests passed." : "Fuzz tests failed.");

    out.writeLine("");
    out.writeLine("Running DI smoke test...");
    bool diPassed = testInteractiveWithoutConsole();
    out.writeLine(diPassed ? "DI smoke test passed." : "DI smoke test failed.");

    return (parserPassed && fuzzPassed && diPassed) ? 0 : 1;
}
