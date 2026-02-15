#include "parser.h"
#include <utility>
#include <cctype>
#include <limits>

Parser::Parser(std::string expression)
    : expr_(std::move(expression))
{
}

// Skips whitespace.
void Parser::skipWhitespace()
{
    while (pos_ < expr_.size() && std::isspace(static_cast<unsigned char>(expr_[pos_])))
    {
        ++pos_;
    }
}

// Reads an integer value and detects overflow safely.
bool Parser::parseInteger(long long& value)
{
    skipWhitespace();

    if (pos_ >= expr_.size() ||
        !std::isdigit(static_cast<unsigned char>(expr_[pos_])))
    {
        error_ = true;
        return false;
    }

    value = 0;

    while (pos_ < expr_.size() &&
           std::isdigit(static_cast<unsigned char>(expr_[pos_])))
    {
        int digit = expr_[pos_] - '0';

        if (value > (std::numeric_limits<long long>::max() - digit) / 10)
        {
            error_ = true;
            return false;
        }

        value = value * 10 + digit;
        ++pos_;
    }

    return true;
}

// Parses the smallest unit of an expression:
//  - integer values
//  - parenthesised expressions
//  - unary + and -
long long Parser::parseValue()
{
    skipWhitespace();

    bool negate = false;

    // Handle + or -.
    if (pos_ < expr_.size() && (expr_[pos_] == '+' || expr_[pos_] == '-'))
    {
        negate = (expr_[pos_] == '-');
        ++pos_;
        skipWhitespace();
    }

    long long value = 0;

    // If we see '(' then parse inner expression.
    if (pos_ < expr_.size() && expr_[pos_] == '(')
    {
        ++pos_; // Skip '('.

        value = parseExpression();

        skipWhitespace();

        if (pos_ >= expr_.size() || expr_[pos_] != ')')
        {
            error_ = true;
            return 0;
        }

        ++pos_; // Skip ')'.
    }
    // Otherwise parse an integer.
    else
    {
        if (!parseInteger(value))
        {
            return 0;
        }
    }

    return negate ? -value : value;
}

// Evaluates expressions left to right using + - * / (no precedence).
long long Parser::parseExpression()
{
    long long result = parseValue();

    if (error_)
    {
        return 0;
    }

    while (!error_)
    {
        const char op = parseOperator();
        if (op == '\0')
        {
            break;
        }

        const long long value = parseValue();
        if (error_)
        {
            return 0;
        }

        switch (op)
        {
            case '+': result += value; break;
            case '-': result -= value; break;
            case '*': result *= value; break;
            case '/':
                if (value == 0)
                {
                    error_ = true;
                    return 0;
                }
                result /= value;
                break;
        }
    }

    return result;
}

// Reads the next operator if present.
char Parser::parseOperator()
{
    skipWhitespace();

    if (pos_ >= expr_.size())
    {
        return '\0';
    }

    const char c = expr_[pos_];
    if (c == '+' || c == '-' || c == '*' || c == '/')
    {
        ++pos_;
        return c;
    }

    return '\0';
}

// Entry point that validates and evaluates the full expression.
bool Parser::evaluate(int& result)
{
    long long temp = parseExpression();

    skipWhitespace();

    if (error_ || pos_ != expr_.size())
    {
        result = 0;
        return false;
    }

    if (temp < std::numeric_limits<int>::min() ||
        temp > std::numeric_limits<int>::max())
    {
        result = 0;
        return false;
    }

    result = static_cast<int>(temp);
    return true;
}
