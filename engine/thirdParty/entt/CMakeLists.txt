include(FetchContent)

FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.16.0
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(entt)
add_library(entt_wrapper INTERFACE)

target_namespace_headers(entt_wrapper "third_party/entt" "${entt_SOURCE_DIR}/src/entt")