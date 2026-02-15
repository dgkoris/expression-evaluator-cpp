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

bool Parser::evaluate(int& result)
{
    result = 0;
    return false;
}
