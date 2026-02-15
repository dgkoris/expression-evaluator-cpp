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

    // If we see '(' then parse inner expression.
    if (pos_ < expr_.size() && expr_[pos_] == '(')
    {
        ++pos_; // Skip '('.

        int value = parseExpression();

        skipWhitespace();

        if (pos_ >= expr_.size() || expr_[pos_] != ')')
        {
            error_ = true;
            return 0;
        }

        ++pos_; // Skip ')'.
        return value;
    }

    // Otherwise parse an integer.
    int value = 0;
    if (!parseInteger(value))
    {
        return 0;
    }

    return value;
}

int Parser::parseExpression()
{
    int value = parseValue();

    if (error_)
    {
        return 0;
    }

    return value;
}

bool Parser::evaluate(int& result)
{
    result = parseExpression();

    skipWhitespace();

    return !error_ && pos_ == expr_.size();
}
