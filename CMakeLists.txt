cmake_minimum_required(VERSION 4.0)
project(DxGraphicsEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(engine)

add_executable(DxGraphicsApp main.cpp)
target_link_libraries(DxGraphicsApp PRIVATE Engine)