#include "parser.h"
#include <utility>
#include <cctype>

Parser::Parser(std::string expression)
    : expr_(std::move(expression))
{
}

void Parser::skipWhitespace()
{
    while (pos_ < expr_.size() && std::isspace(static_cast<unsigned char>(expr_[pos_])))
    {
        ++pos_;
    }
}

bool Parser::parseInteger(int& value)
{
    skipWhitespace();

    bool negative = false;
    if (pos_ < expr_.size() && (expr_[pos_] == '+' || expr_[pos_] == '-'))
    {
        negative = (expr_[pos_] == '-');
        ++pos_;
    }

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
        value = value * 10 + (expr_[pos_] - '0');
        ++pos_;
    }

    if (negative)
    {
        value = -value;
    }

    return true;
}

int Parser::parseValue()
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

    int value = 0;

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

int Parser::parseExpression()
{
    int result = parseValue();

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

        const int value = parseValue();
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

bool Parser::evaluate(int& result)
{
    result = parseExpression();

    skipWhitespace();

    return !error_ && pos_ == expr_.size();
}
