#pragma once
#define SLOG_DEFAULT_FILE_SINK 0
#include <slog.hpp>
#include <vector>

struct LogRecord
{
public:
    slog::Context ctx;
    slog::Severity sev;
    std::string msg;
};

class MockSink: public slog::Sink
{
public:
    std::vector<LogRecord> records;
    void record() override
    {

    }
}