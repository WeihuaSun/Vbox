# cmake_minimum_required(VERSION 3.10)

# set(CMAKE_CXX_COMPILER "g++")
# project(SerVerifier)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(MONOSAT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/monosat")
# set(MONOSAT_BUILD_DIR "${PROJECT_SOURCE_DIR}/monosat")
# include_directories("/usr/local/include/monosat")

# include_directories(
#     ${PROJECT_SOURCE_DIR}/src
#     ${PROJECT_SOURCE_DIR}/kissat/src
#     ${PROJECT_SOURCE_DIR}/cadical/src
# )
# set(SOURCES
#     src/transaction/transaction.cpp
#     src/graph/graph.cpp
#     src/graph/transitive_closure.cpp
#     src/verifier/vbox.cpp
#     src/verifier/leopard.cpp
#     src/solver/constraint.cpp
#     src/solver/solver.cpp
#     src/main.cpp
# )


# add_executable(SerVerifier ${SOURCES})


# find_library(CADICAL_LIB cadical
#     PATHS ${PROJECT_SOURCE_DIR}/cadical/build
#     NO_DEFAULT_PATH
# )

# if(NOT CADICAL_LIB)
#     message(FATAL_ERROR "Could not find libcadical.a")
# endif()

# find_library(KISSAT_LIB kissat
#     PATHS ${PROJECT_SOURCE_DIR}/kissat/build
#     NO_DEFAULT_PATH
# )

# if(NOT KISSAT_LIB)
#     message(FATAL_ERROR "Could not find libkissat.a")
# endif()

# find_library(MONOSAT_LIB monosat /usr/local/lib)

# if(NOT MONOSAT_LIB)
#     message(FATAL_ERROR "Could not find MonoSAT library.")
# endif()

# if(NOT MONOSAT_LIB)
#     message(FATAL_ERROR "The specified MonoSAT library does not exist.")
# endif()

# target_link_libraries(SerVerifier
#     ${CADICAL_LIB}
#     ${MONOSAT_LIB}
#     ${KISSAT_LIB}
# )

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic  -O3")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")
# set(CMAKE_BUILD_TYPE Release)


# cmake_minimum_required(VERSION 3.10)
# set(CMAKE_CXX_COMPILER "g++")

# project(SerVerifier)
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_BUILD_TYPE Release)

# # 添加你的头文件目录
# include_directories(
#     ${PROJECT_SOURCE_DIR}/src
#     ${PROJECT_SOURCE_DIR}/kissat/src
#     ${PROJECT_SOURCE_DIR}/cadical/src
# )

# # set(MONOSAT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/monosat")
# # set(MONOSAT_BUILD_DIR "${PROJECT_SOURCE_DIR}/monosat")
# # include_directories("/usr/local/include/monosat")
# # include_directories("/usr/local/include/monosat")
# # 添加源文件
# set(SOURCES
#     src/transaction/transaction.cpp
#     src/graph/graph.cpp
#     src/graph/transitive_closure.cpp
#     src/verifier/vbox.cpp
#     src/verifier/leopard.cpp
#     src/solver/constraint.cpp
#     src/solver/solver.cpp
#     src/main.cpp
# )

# add_executable(SerVerifier ${SOURCES})

# # 指定手动构建的静态库路径（CaDiCaL 和 Kissat）
# target_link_libraries(SerVerifier
#     ${PROJECT_SOURCE_DIR}/cadical/build/libcadical.a
#     ${PROJECT_SOURCE_DIR}/kissat/build/libkissat.a
# )

# find_library(MONOSAT_LIB monosat /usr/local/lib)


# if(NOT MONOSAT_LIB)
#     message(FATAL_ERROR "The specified MonoSAT library does not exist.")
# endif()

# target_link_libraries(SerVerifier
#     ${PROJECT_SOURCE_DIR}/cadical/build/libcadical.a
#     ${PROJECT_SOURCE_DIR}/kissat/build/libkissat.a
#     ${MONOSAT_LIB}
# )

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic  -O3")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_COMPILER "g++")

project(SerVerifier)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)

# 添加头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/kissat/src
    ${PROJECT_SOURCE_DIR}/cadical/src
)

# 添加源文件
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

# ======= 查找并链接 CaDiCaL =======
find_library(CADICAL_LIB cadical
    PATHS ${PROJECT_SOURCE_DIR}/cadical/build
    NO_DEFAULT_PATH
)

if(NOT CADICAL_LIB)
    message(FATAL_ERROR "Could not find libcadical.a")
endif()

# ======= 查找并链接 Kissat =======
find_library(KISSAT_LIB kissat
    PATHS ${PROJECT_SOURCE_DIR}/kissat/build
    NO_DEFAULT_PATH
)

if(NOT KISSAT_LIB)
    message(FATAL_ERROR "Could not find libkissat.a")
endif()

# ======= 查找并链接 MonoSAT =======
find_library(MONOSAT_LIB monosat /usr/local/lib)

if(NOT MONOSAT_LIB)
    message(FATAL_ERROR "Could not find MonoSAT library.")
endif()

# 一次性链接所有库
target_link_libraries(SerVerifier
    ${CADICAL_LIB}
    ${MONOSAT_LIB}
    ${KISSAT_LIB}
)

# 编译器和链接器标志
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O3")
