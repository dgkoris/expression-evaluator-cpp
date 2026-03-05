#include "parser.h"
#include <utility>
#include <cctype>
#include <limits>

Parser::Parser(std::string expression)
    : expr_(std::move(expression))
{
}

bool Parser::evaluate(int& result) const
{
    Parser copy(*this);
    return copy.evaluate(result);
}

std::string Parser::lastError() const
{
    return errorMsg_;
}

std::size_t Parser::lastErrorPosition() const
{
    return errorPos_;
}

void Parser::setError(const std::string& message)
{
    if (!error_)
    {
        error_ = true;
        errorPos_ = pos_;
        errorMsg_ = message;
    }
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

    if (pos_ >= expr_.size() || !std::isdigit(static_cast<unsigned char>(expr_[pos_])))
    {
        setError("Expected a number");
        return false;
    }

    value = 0;

    while (pos_ < expr_.size() && std::isdigit(static_cast<unsigned char>(expr_[pos_])))
    {
        int digit = expr_[pos_] - '0';

        if (value > (std::numeric_limits<long long>::max() - digit) / 10)
        {
            setError("Number too large");
            return false;
        }

        value = value * 10 + digit;
        ++pos_;
    }

    return true;
}

bool Parser::safeAdd(long long a, long long b, long long& out)
{
    if ((b > 0 && a > std::numeric_limits<long long>::max() - b) ||
        (b < 0 && a < std::numeric_limits<long long>::min() - b))
    {
        setError("Overflow in addition");
        return false;
    }

    out = a + b;
    return true;
}

bool Parser::safeSub(long long a, long long b, long long& out)
{
    if ((b > 0 && a < std::numeric_limits<long long>::min() + b) ||
        (b < 0 && a > std::numeric_limits<long long>::max() + b))
    {
        setError("Overflow in subtraction");
        return false;
    }

    out = a - b;
    return true;
}

bool Parser::safeMul(long long a, long long b, long long& out)
{
    if (a == 0 || b == 0)
    {
        out = 0;
        return true;
    }

    const long long max = std::numeric_limits<long long>::max();
    const long long min = std::numeric_limits<long long>::min();

    // Special case: min * -1 overflows
    if ((a == min && b == -1) || (b == min && a == -1))
    {
        setError("Overflow in multiplication");
        return false;
    }

    if (a > 0)
    {
        if (b > 0)
        {
            if (a > max / b)
            {
                setError("Overflow in multiplication");
                return false;
            }
        }
        else
        {
            if (b < min / a)
            {
                setError("Overflow in multiplication");
                return false;
            }
        }
    }
    else
    {
        if (b > 0)
        {
            if (a < min / b)
            {
                setError("Overflow in multiplication");
                return false;
            }
        }
        else
        {
            // a < 0, b < 0
            if (a < max / b)
            {
                setError("Overflow in multiplication");
                return false;
            }
        }
    }

    out = a * b;
    return true;
}

bool Parser::safeNegate(long long v, long long& out)
{
    if (v == std::numeric_limits<long long>::min())
    {
        setError("Overflow in unary negation");
        return false;
    }

    out = -v;
    return true;
}

// Parses the smallest unit of an expression:
//  - integer values
//  - parenthesised expressions
//  - unary + and -
long long Parser::parseValue()
{
    skipWhitespace();

    bool hasSign = false;
    bool negate = false;

    if (pos_ < expr_.size() && (expr_[pos_] == '+' || expr_[pos_] == '-'))
    {
        hasSign = true;
        negate = (expr_[pos_] == '-');
        ++pos_;
        skipWhitespace();

        // Reject double sign like "--5" "+-5" "-+5"
        if (pos_ < expr_.size() && (expr_[pos_] == '+' || expr_[pos_] == '-'))
        {
            setError("Unexpected second sign");
            return 0;
        }
    }

    long long value = 0;

    if (pos_ < expr_.size() && expr_[pos_] == '(')
    {
        ++pos_; // Skip '('

        value = parseExpression();
        if (error_)
        {
            return 0;
        }

        skipWhitespace();

        if (pos_ >= expr_.size() || expr_[pos_] != ')')
        {
            setError("Missing ')'");
            return 0;
        }

        ++pos_; // Skip ')'
    }
    else
    {
        if (!parseInteger(value))
        {
            return 0;
        }
    }

    if (hasSign && negate)
    {
        long long neg = 0;
        if (!safeNegate(value, neg))
        {
            return 0;
        }
        return neg;
    }

    return value;
}

long long Parser::parseTerm()
{
    long long result = parseValue();
    if (error_)
    {
        return 0;
    }

    while (!error_)
    {
        skipWhitespace();

        if (pos_ >= expr_.size())
        {
            break;
        }

        const char op = expr_[pos_];
        if (op != '*' && op != '/')
        {
            break;
        }

        ++pos_;

        const long long rhs = parseValue();
        if (error_)
        {
            return 0;
        }

        if (op == '*')
        {
            long long out = 0;
            if (!safeMul(result, rhs, out))
            {
                return 0;
            }
            result = out;
        }
        else
        {
            if (rhs == 0)
            {
                setError("Division by zero");
                return 0;
            }
            result /= rhs;
        }
    }

    return result;
}

long long Parser::parseExpression()
{
    long long result = parseTerm();
    if (error_)
    {
        return 0;
    }

    while (!error_)
    {
        skipWhitespace();

        if (pos_ >= expr_.size())
        {
            break;
        }

        const char op = expr_[pos_];
        if (op != '+' && op != '-')
        {
            break;
        }

        ++pos_;

        const long long rhs = parseTerm();
        if (error_)
        {
            return 0;
        }

        long long out = 0;

        if (op == '+')
        {
            if (!safeAdd(result, rhs, out))
            {
                return 0;
            }
        }
        else
        {
            if (!safeSub(result, rhs, out))
            {
                return 0;
            }
        }

        result = out;
    }

    return result;
}

// Entry point that validates and evaluates the full expression.
bool Parser::evaluate(int& result)
{
    // Reset state so the same Parser instance can evaluate multiple times
    pos_ = 0;
    error_ = false;
    errorPos_ = 0;
    errorMsg_.clear();

    const long long temp = parseExpression();

    skipWhitespace();

    if (error_ || pos_ != expr_.size())
    {
        if (!error_)
        {
            setError("Unexpected trailing characters");
        }

        result = 0;
        return false;
    }

    if (temp < std::numeric_limits<int>::min() || temp > std::numeric_limits<int>::max())
    {
        setError("Result out of int range");
        result = 0;
        return false;
    }

    result = static_cast<int>(temp);
    return true;
}
