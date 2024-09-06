cmake_minimum_required(VERSION 3.11)

project(slog
    VERSION 1.0
)
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

add_library(slog INTERFACE)
target_include_directories(slog INTERFACE inc/)
target_sources(slog INTERFACE ${CMAKE_SOURCE_DIR}/inc/slog.hpp)

add_executable(slog_demo_cpp11 slog_demo.cpp)
target_link_libraries(slog_demo_cpp11 slog fmt::fmt)
add_executable(slog_demo_cpp17 slog_demo.cpp)
target_link_libraries(slog_demo_cpp17 slog fmt::fmt)
target_compile_features(slog_demo_cpp17 PUBLIC cxx_std_17)


