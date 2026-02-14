include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

add_library(glfw_wrapper INTERFACE)
target_link_libraries(glfw_wrapper INTERFACE glfw)


target_namespace_headers(glfw_wrapper "third_party/glfw" "${glfw_SOURCE_DIR}/include/GLFW")