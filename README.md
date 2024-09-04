# Salty Logger
saltyJeff's C++ logger

My attempt at a C++ logging library that won't be larger than the rest of your code.


## Installation
Single file header-only, include [`./inc/slog.hpp`](./inc/slog.hpp) in your project.

## Usage
### Logging with the provided console logger
```c++
// style 1: iostream-style logging
SLOG(DEBUG) << "foo bar";

// style 2: include the string as part of the method call
SLOG(DEBUG, "foo bar");

// style 3: use formatting (if fmtlib or std::format are available)
SLOG(DEBUG, "foo {}", "bar"); // bar will be substituted into {}
```
See [slog_demo.cpp](./slog_demo.cpp) for a demo

### Logging with a custom log sink
```c++
// first, implement the record() method of the slog::Sink interface
class FooSink: public Sink
{
public:
    void record(Severity sev, const Context &ctx, const std::string &msg) override
    {
        // TODO
    }
}

// create an instance of your sink
FooSink sink;

// use the above SLOG methods passing a reference to the instance
SLOG(DEBUG, sink) << "i'm going to foo logger";

// to replace the default logger (so you don't have to pass &sink)
slog::DEFAULT_SINK().reset(new FooSink());
```

## Features
* small, currently < 200 lines including comments and docstrings
* support for C++11 onwards
* supports [`fmtlib`](https://github.com/fmtlib/fmt/tree/master) with no configuration on C++17, or with the `SLOG_USE_FMTLIB` symbol defined
* supports [`std::format`](https://en.cppreference.com/w/cpp/utility/format/format) with no configuration on C++20, or with the `SLOG_USE_STDFMT` symbol defined
