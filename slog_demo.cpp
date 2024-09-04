#include <slog.hpp>

int main()
{
    SLOG(ERROR) << "this is an error in stream form";
    SLOG(WARN, "this is a warning in string form");
    SLOG(INFO, "this is an {} in {} form", "info", "format");
    // test this issue: https://stackoverflow.com/questions/5028302/small-logger-class#comment69428442_5028917
    for(int i = 0; i < 5; i++)
    {
        SLOG(DEBUG, "this is a debug log: {:d}", i);
    }
    SLOG_IF(ERROR, false) << "this doesn't exist";
}