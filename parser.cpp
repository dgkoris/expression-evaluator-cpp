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

bool Parser::evaluate(int& result)
{
    if (!parseInteger(result))
    {
        result = 0;
        return false;
    }

    skipWhitespace();

    return pos_ == expr_.size();
}
