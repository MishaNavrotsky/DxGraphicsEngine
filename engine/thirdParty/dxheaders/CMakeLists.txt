include(FetchContent)

FetchContent_Declare(
        directx_headers
        GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
        GIT_TAG main
)
FetchContent_MakeAvailable(directx_headers)

add_library(dxheaders_wrapper INTERFACE)

target_namespace_headers(dxheaders_wrapper "third_party/directx" "${directx_headers_SOURCE_DIR}/include/directx")
target_include_directories(dxheaders_wrapper INTERFACE "${directx_headers_SOURCE_DIR}/include/directx")