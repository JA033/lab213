cmake_minimum_required(VERSION 3.15)

project(GeCSA VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(GECSA64_DIVSUFSORT_SOURCES
    "divsufsort/config.h"
    "divsufsort/divsufsort_private.h"
    "divsufsort/divsufsort.c"
    "divsufsort/divsufsort.h"
    "divsufsort/divsufsort64.h"
    "divsufsort/makefile"
    "divsufsort/sssort.c"
    "divsufsort/trsort.c"
    "divsufsort/utils.c"
)

add_library(divsufsort64 ${GECSA64_DIVSUFSORT_SOURCES})
target_include_directories(divsufsort64 PUBLIC "divsufsort")
target_compile_definitions(divsufsort64 PRIVATE HAVE_CONFIG_H BUILD_DIVSUFSORT64=1)

option(GECSA64_TEST_CSA_DMAP "Enable testing the D-map of GeCSA" OFF)
option(GECSA64_TEST_CSA_PHI "Enable testing the phi-array of GeCSA" OFF)

set(GECSA64_SOURCES
    "ABS_Phi.h"
    "BaseClass.h"
    "broadword.hpp"
    "CSA.cpp"
    "CSA.h"
    "Gen_Phi.cpp"
    "Gen_Phi.h"
    "InArray.cpp"
    "InArray.h"
    "intrinsics.hpp"
    "loadkit.cpp"
    "loadkit.h"
    "macros.h"
    "parmaters.h"
    "Phi.cpp"
    "Phi.h"
    "savekit.cpp"
    "savekit.h"
    "select.h"
    "tables.hpp"
)

add_library(GeCSA64 ${GECSA64_SOURCES})

target_link_libraries(GeCSA64 PRIVATE divsufsort64)
target_include_directories(GeCSA64 PUBLIC .)
target_compile_options(GeCSA64 PRIVATE -Wall -Wextra -Wno-sign-compare)

if(GECSA64_TEST_CSA_DMAP)
    target_compile_definitions(GeCSA64 PRIVATE GECSA_TEST_CSA_DMAP)
endif()

if(GECSA64_TEST_CSA_PHI)
    target_compile_definitions(GeCSA64 PRIVATE GECSA_TEST_CSA_PHI)
endif()