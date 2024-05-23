#pragma once
#include "ConcurrentQueue.h"
#include <string>
#include <chrono>

struct Loggable
{
    std::chrono::system_clock::time_point timestamp;
    std::string blockname;
    std::string content;

    Loggable(const std::string &content, std::string blockname = "")
        : timestamp(std::chrono::system_clock::now()), blockname(blockname), content(content) {}
    Loggable() = default;
};

class SystemLogger
{
public:
    void Log(const std::string &line, std::string blockname = "");
    bool TryPop(Loggable &loggable);

private:
    ConcurrentQueue<Loggable> lines;
};

void SystemLogger::Log(const std::string &line, std::string blockname)
{
    Loggable loggable(line, blockname);
    lines.Push(loggable);
}

bool SystemLogger::TryPop(Loggable &loggable)
{
    return lines.TryPop(loggable);
}
