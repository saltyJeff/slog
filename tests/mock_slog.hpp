#pragma once
#define SLOG_FILE_SINK_DEFAULT 0
#include <slog.hpp>
#include <vector>

struct LogRecord
{
public:
    slog::Severity sev;
    slog::Context ctx;
    std::string msg;
};

class MockSink : public slog::Sink
{
public:
    std::vector<LogRecord> records;
    void record(slog::Severity sev, const slog::Context &ctx, const std::string &msg) override { records.push_back(LogRecord{sev, ctx, msg}); }
};

inline slog::Sink &slog::DEFAULT_SINK()
{
    static MockSink sink;
    return sink;
}