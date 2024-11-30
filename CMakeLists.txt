cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER "g++")
project(SerVerifier)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(MONOSAT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/monosat")
set(MONOSAT_BUILD_DIR "${PROJECT_SOURCE_DIR}/monosat")
include_directories("/usr/local/include/monosat")

include_directories(
    ${PROJECT_SOURCE_DIR}/src
)


set(SOURCES
    src/transaction/transaction.cpp
    src/graph/graph.cpp
    src/graph/transitive_closure.cpp
    src/verifier/vbox.cpp
    src/verifier/leopard.cpp
    src/solver/constraint.cpp
    src/solver/solver.cpp
    src/main.cpp
)


add_executable(SerVerifier ${SOURCES})


find_library(MONOSAT_LIB monosat /usr/local/lib)


if(NOT MONOSAT_LIB)
    message(FATAL_ERROR "The specified MonoSAT library does not exist.")
endif()
target_link_libraries(SerVerifier ${MONOSAT_LIB})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic  -O3")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")
set(CMAKE_BUILD_TYPE Release)
