cmake_minimum_required(VERSION 3.15)

project(CSA-Wildcard VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

find_package(TBB REQUIRED)

add_subdirectory(deps/GeCSA-Wrapper)

add_library(wildcard STATIC
    "source/document_searcher.cpp"
    "source/utils.cpp"
    "source/wildcard_executor.cpp"
    "source/wildcard_pattern.cpp"
    "source/searchable_bitmap.cpp"
)

target_link_libraries(wildcard PUBLIC gecsa TBB::tbb)
target_compile_options(wildcard PRIVATE -march=native)

add_executable(csa_wildcard source/main.cpp)
target_link_libraries(csa_wildcard PRIVATE wildcard)
