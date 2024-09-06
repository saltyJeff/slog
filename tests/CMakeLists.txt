enable_testing()
include(CTest)
include(doctest.cmake)

add_library(mock_slog INTERFACE)
target_include_directories(mock_slog INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(mock_slog INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mock_slog.hpp)
target_link_libraries(mock_slog INTERFACE slog)

add_executable(test_cpp11 test_cpp11.cpp)
target_link_libraries(test_cpp11 mock_slog)
target_compile_features(test_cpp11 PUBLIC cxx_std_17)
