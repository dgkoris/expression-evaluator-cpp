#pragma once
#include <cstddef>
#include <string>

class Parser
{
public:
    explicit Parser(std::string expression);
    bool evaluate(int& result);
    bool evaluate(int& result) const;
    std::string lastError() const;
    std::size_t lastErrorPosition() const;

private:
    std::string expr_;
    std::size_t pos_ = 0;
    bool error_ = false;
    std::size_t errorPos_ = 0;
    std::string errorMsg_;

    void setError(const std::string& message);
    void skipWhitespace();

    bool parseInteger(long long& value);
    long long parseValue();
    long long parseTerm();
    long long parseExpression();

    // Overflow-safe helpers for evaluation
    bool safeAdd(long long a, long long b, long long& out);
    bool safeSub(long long a, long long b, long long& out);
    bool safeMul(long long a, long long b, long long& out);
    bool safeNegate(long long v, long long& out);
};
