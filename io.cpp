#include "io.h"
#include <iostream>
#include <utility>

void ConsoleWriter::writeLine(const std::string& line)
{
    std::cout << line << '\n';
}

bool ConsoleReader::readLine(std::string& out)
{
    return static_cast<bool>(std::getline(std::cin, out));
}

void StringWriter::writeLine(const std::string& line)
{
    lines_.push_back(line);
}

const std::vector<std::string>& StringWriter::lines() const
{
    return lines_;
}

VectorReader::VectorReader(std::vector<std::string> lines)
    : lines_(std::move(lines))
{
}

bool VectorReader::readLine(std::string& out)
{
    if (index_ >= lines_.size())
    {
        return false;
    }

    out = lines_[index_];
    ++index_;
    return true;
}
