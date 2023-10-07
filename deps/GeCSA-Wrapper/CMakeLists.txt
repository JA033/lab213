cmake_minimum_required(VERSION 3.15)

project(gecsa VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

option(GECSA_TEST_CSA_DMAP "Enable testing the D-map of GeCSA" OFF)
option(GECSA_TEST_CSA_PHI "Enable testing the phi-array of GeCSA" OFF)


if(GECSA_TEST_CSA_DMAP)
    set(GECSA64_TEST_CSA_DMAP ON CACHE BOOL "Enable testing the D-map of GeCSA" FORCE)
endif()

if(GECSA_TEST_CSA_PHI)
    set(GECSA64_TEST_CSA_PHI ON CACHE BOOL "Enable testing the phi-array of GeCSA" FORCE)
endif()

add_subdirectory(source/GeCSA64)

set(GECSA_SOURCES
    "source/bitmap.cpp"
    "source/gecsa.cpp"
    "source/int_array.cpp"
    "source/sparse_bitmap.cpp"
)

add_library(gecsa STATIC ${GECSA_SOURCES})
add_library(gecsa::gecsa ALIAS gecsa)

target_include_directories(gecsa PUBLIC include)
target_link_libraries(gecsa PRIVATE GeCSA64)

