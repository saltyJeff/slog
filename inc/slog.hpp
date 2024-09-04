/**
 * @file slog.hpp
 * @author saltyJeff (saltyJeff@users.noreply.github.com)
 * @brief saltyLogger: My attempt at a tiny header-only C++11+ logger
 * @license MIT
 */
#pragma once
#ifndef SLOG_HPP_
#define SLOG_HPP_
// header options. You can edit this file directly, or add the appropriate preprocessor defines
#ifndef SLOG_HIDE_SRC // sets whether the source code info will be hidden with dummy values
#define SLOG_HIDE_SRC 0
#endif
#ifndef SLOG_DEFAULT_FILE_SINK // sets whether the FileSink will be defined and made the default
#define SLOG_DEFAULT_FILE_SINK 1
#endif
#ifndef SLOG_FMT // sets whether fmt-lib style logging is supported (0 for disabled, 1 for fmtlib, 2 for stdfmt)
#define SLOG_FMT 1*(__cplusplus >= 201703L && __has_include(<fmt/format.h>))
#if SLOG_FMT == 0
#define SLOG_FMT 2*(__cplusplus >= 202002L && __has_include(<format>))
#endif
#endif
// uncomment below to strip all comments below WARN level
//#define SLOG_STRIP_BELOW WARN

#if SLOG_FMT == 1
#define SLOG_FMT_NS fmt
#include <fmt/format.h>
#elif SLOG_FMT == 2
#define SLOG_FMT_NS std
#include <format>
#endif

#include <memory>
#include <string>

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
struct Context;
/** An interface for a log sink. Implement the record method */
class Sink
{
  public:
    virtual void record(Severity sev, const Context &ctx, const std::string &msg) = 0;
};
} // namespace slog
#include <ctime>
#include <sstream>
#include <thread>

namespace slog
{
/** Context of each log message */
struct Context
{
  public:
    std::time_t time;
    std::thread::id thread_id;
    const char *file_name;
    unsigned int line;
    const char *func_name;
    Context(const char *file_name, unsigned int line, const char *func_name)
        : thread_id(std::this_thread::get_id()), file_name(file_name), line(line), func_name(func_name)
    {
        std::time(&time);
    }
};
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

#if SLOG_DEFAULT_FILE_SINK == 1
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
inline std::shared_ptr<Sink> &DEFAULT_SINK()
{
    static std::shared_ptr<Sink> sink{new FileSink()};
    return sink;
}
#else
extern std::shared_ptr<Sink> &DEFAULT_SINK();
#endif

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

#if SLOG_FMT == 1 || SLOG_FMT == 2
template <typename... T> inline void log_impl(Context &&ctx, Severity sev, SLOG_FMT_NS::format_string<T...> fmt, T &&...args)
{
    DEFAULT_SINK()->record(sev, ctx, SLOG_FMT_NS::format(fmt, std::forward<T>(args)...));
};
template <typename... T> inline void log_impl(Context &&ctx, Sink &sink, Severity sev, SLOG_FMT_NS::format_string<T...> fmt, T &&...args)
{
    sink.record(sev, ctx, SLOG_FMT_NS::format(fmt, std::forward<T>(args)...));
};
#endif
} // namespace slog
#if SLOG_HIDE_SRC == 1
#define SLOG_CTX()                                                                                                                                   \
    slog::Context                                                                                                                                    \
    {                                                                                                                                                \
        "?", 0, "?"                                                                                                                                  \
    }
#else
#define SLOG_CTX()                                                                                                                                   \
    slog::Context                                                                                                                                    \
    {                                                                                                                                                \
        __FILE__, __LINE__, __PRETTY_FUNCTION__                                                                                                      \
    }
#endif
#define SLOG(SEV, ...) slog::log_impl(SLOG_CTX(), slog::Severity::SEV, ##__VA_ARGS__)

#endif