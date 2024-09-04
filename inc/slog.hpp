/**
 * @file slog.hpp
 * @author saltyJeff (saltyJeff@users.noreply.github.com)
 * @brief saltyLogger: My attempt at a tiny header-only C++11+ logger
 * @license MIT
 */
#pragma once
#ifndef SLOG_HPP_
#define SLOG_HPP_
#include <cstdio>
#define __STDC_WANT_LIB_EXT1__ 1
#include <ctime>
#include <memory>
#include <sstream>
#include <string>

#ifdef SLOG_NO_FMT
// allow users to disable loading the format headers
#elif SLOG_USE_FMTLIB || (__cplusplus >= 201703L && __has_include(<fmt/format.h>))
#define SLOG_FMT 1
#include <fmt/format.h>
#define SLOG_FMT_NS fmt
#elif SLOG_USE_STDFMT || (__cplusplus >= 202002L && __has_include(<format>))
#define SLOG_FMT 1
#include <format>
#define SLOG_FMT_NS std
#endif

namespace slog
{
/** The severity of the logger */
enum class Severity
{
    DEBUG,
    INFO,
    WARN,
    ERROR
};
inline const char *severity_to_str(Severity sev)
{
    switch (sev)
    {
    case Severity::DEBUG:
        return "DEBUG";
    case Severity::INFO:
        return "INFO";
    case Severity::WARN:
        return "WARN";
    case Severity::ERROR:
        return "ERROR";
    default:
        return "?";
    }
}
/** Context of each log message */
struct Context
{
  public:
    std::time_t time;
    const char *file_name;
    unsigned int line;
    const char *func_name;
    Context(const char *file_name, unsigned int line, const char *func_name)
        : file_name(file_name), line(line), func_name(func_name)
    {
        std::time(&time);
    }
};
/** An interface for a log sink. Implement the record method */
class Sink
{
  public:
    virtual void record(Severity sev, const Context &ctx, const std::string &msg) = 0;
};
/** An implementation of the sink that goes to a FILE* */
class FileSink : public Sink
{
  private:
    std::FILE *file;
    bool close_dtor;

  public:
    FileSink(std::FILE *file = stderr, bool close_dtor = false) : file(file), close_dtor(close_dtor) {};
    void record(Severity sev, const Context &ctx, const std::string &msg) override
    {
        char time_str[32];
        tm time_buf;
        std::strftime(time_str, sizeof(time_str), "%Y-%m-%dT%H:%M:%S", ::localtime_r(&ctx.time, &time_buf));
        fprintf(file, "%s\t%s\t%s\n", time_str, severity_to_str(sev), msg.c_str());
    }
    ~FileSink()
    {
        if (close_dtor)
        {
            fclose(file);
        }
    }
};
inline std::unique_ptr<Sink> &DEFAULT_SINK()
{
    static std::unique_ptr<Sink> sink{new FileSink()};
    return sink;
}

class LogObjStream
{
  private:
    const Context ctx;
    const Severity sev;
    Sink &sink;
    std::ostringstream msg;

  public:
    LogObjStream(Context &&ctx, Severity sev, Sink &sink) : ctx(ctx), sev(sev), sink(sink) {};
    LogObjStream(LogObjStream &&) = default;
    template <typename T> LogObjStream &operator<<(const T &t)
    {
        msg << t;
        return *this;
    }
    ~LogObjStream()
    {
        sink.record(sev, ctx, msg.str());
    }
};
inline LogObjStream log_impl(Context &&ctx, Severity sev, Sink &sink = *DEFAULT_SINK())
{
    return LogObjStream(std::move(ctx), sev, sink);
};
inline void log_impl(Context &&ctx, Severity sev, const std::string &msg)
{
    DEFAULT_SINK()->record(sev, ctx, msg);
};
inline void log_impl(Context &&ctx, Severity sev, Sink &sink, const std::string &msg)
{
    sink.record(sev, ctx, msg);
};
#ifdef SLOG_FMT
template <typename... T>
inline void log_impl(Context &&ctx, Severity sev, SLOG_FMT_NS::format_string<T...> fmt, T &&...args)
{
    DEFAULT_SINK()->record(sev, ctx, SLOG_FMT_NS::format(fmt, std::forward<T>(args)...));
};
template <typename... T>
inline void log_impl(Context &&ctx, Sink &sink, Severity sev, SLOG_FMT_NS::format_string<T...> fmt, T &&...args)
{
    sink.record(sev, ctx, SLOG_FMT_NS::format(fmt, std::forward<T>(args)...));
};
#endif
} // namespace slog

#define SLOG_CTX()                                                                                                     \
    slog::Context                                                                                                      \
    {                                                                                                                  \
        __FILE__, __LINE__, __PRETTY_FUNCTION__                                                                        \
    }
#define SLOG(SEV, ...) slog::log_impl(SLOG_CTX(), slog::Severity::SEV, ##__VA_ARGS__)

#endif