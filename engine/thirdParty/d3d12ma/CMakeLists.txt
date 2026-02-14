include(FetchContent)

FetchContent_Declare(
        d3d12ma
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(d3d12ma)

add_library(d3d12ma_wrapper STATIC
        "${d3d12ma_SOURCE_DIR}/src/D3D12MemAlloc.cpp"
        "${d3d12ma_SOURCE_DIR}/include/D3D12MemAlloc.h"
)

target_include_directories(d3d12ma_wrapper PRIVATE
        "${d3d12ma_SOURCE_DIR}/include"
        "${d3d12ma_SOURCE_DIR}/src"
)

target_namespace_headers(d3d12ma_wrapper "third_party/d3d12ma" "${d3d12ma_SOURCE_DIR}/include")

target_link_libraries(d3d12ma_wrapper PUBLIC d3d12 dxgi)