#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "mock_slog.hpp"

TEST_CASE("method-style logging works")
{
    int line = -1;
    MockSink &sink = static_cast<MockSink&>(slog::DEFAULT_SINK());
// clang-format: off
    SLOG(DEBUG, "foo"); line = __LINE__;
// clang-format: on
    LogRecord record = sink.records.back();
    sink.records.pop_back();
    CHECK_EQ(std::string(record.ctx.file_name), __FILE__);
    CHECK_EQ(std::string(record.ctx.func_name), __FUNCTION__);
    CHECK_EQ(record.ctx.line, line);
    CHECK_EQ(record.sev, slog::Severity::DEBUG);
    CHECK_EQ(record.msg, "foo");
}

TEST_CASE("stream-style logging works")
{
    int line = -1;
    MockSink &sink = static_cast<MockSink&>(slog::DEFAULT_SINK());
// clang-format: off
    SLOG(DEBUG) << "foo"; line = __LINE__;
// clang-format: on
    LogRecord record = sink.records.back();
    sink.records.pop_back();
    CHECK_EQ(std::string(record.ctx.file_name), __FILE__);
    CHECK_EQ(std::string(record.ctx.func_name), __FUNCTION__);
    CHECK_EQ(record.ctx.line, line);
    CHECK_EQ(record.sev, slog::Severity::DEBUG);
    CHECK_EQ(record.msg, "foo");
}