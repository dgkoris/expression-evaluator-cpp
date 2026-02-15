#pragma once
#include <cstddef>
#include <string>

class Parser
{
public:
    explicit Parser(std::string expression);
    bool evaluate(int& result);

private:
    std::string expr_;
    std::size_t pos_ = 0;
    bool error_ = false;

    void skipWhitespace();
};
