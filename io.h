#pragma once
#include <cstddef>
#include <string>
#include <vector>

struct IWriter
{
    virtual ~IWriter() = default;
    virtual void writeLine(const std::string& line) = 0;
};

struct IReader
{
    virtual ~IReader() = default;
    virtual bool readLine(std::string& out) = 0;
};

class ConsoleWriter : public IWriter
{
public:
    void writeLine(const std::string& line) override;
};

class ConsoleReader : public IReader
{
public:
    bool readLine(std::string& out) override;
};

class StringWriter : public IWriter
{
public:
    void writeLine(const std::string& line) override;
    const std::vector<std::string>& lines() const;

private:
    std::vector<std::string> lines_;
};

class VectorReader : public IReader
{
public:
    explicit VectorReader(std::vector<std::string> lines);
    bool readLine(std::string& out) override;

private:
    std::vector<std::string> lines_;
    std::size_t index_ = 0;
};
