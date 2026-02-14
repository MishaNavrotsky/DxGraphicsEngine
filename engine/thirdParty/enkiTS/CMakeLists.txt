include(FetchContent)

FetchContent_Declare(
        enkiTS
        GIT_REPOSITORY https://github.com/dougbinks/enkiTS.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(enkiTS)

add_library(enkiTS_wrapper INTERFACE)
target_link_libraries(enkiTS_wrapper INTERFACE enkiTS)

target_namespace_headers(enkiTS_wrapper "third_party/enkiTS" "${enkiTS_SOURCE_DIR}/src")