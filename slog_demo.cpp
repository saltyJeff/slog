#include <slog.hpp>

int main()
{
    SLOG(ERROR) << "this is an error in stream form";
    SLOG(WARN, "this is a warning in string form");
    #if SLOG_FMT > 0 // SLOG_FMT will be non-zero if one of the formatting libraries is detected
    SLOG(INFO, "this is an {} in {} form", "info", "format");
    #endif
    // test this issue: https://stackoverflow.com/questions/5028302/small-logger-class#comment69428442_5028917
    for(int i = 0; i < 16; i++)
    {
        SLOG(DEBUG) << "this is a debug log: " << std::hex << i;
    }
    SLOG_IF(ERROR, false) << "this doesn't exist";
}