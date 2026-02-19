include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.17.0
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(spdlog)
add_library(spdlog_wrapper INTERFACE)
target_link_libraries(spdlog_wrapper INTERFACE spdlog::spdlog_header_only)

target_namespace_headers(spdlog_wrapper "third_party/spdlog" "${spdlog_SOURCE_DIR}/include/spdlog")