cmake_minimum_required(VERSION 3.15)
project(pkmndriver)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect platform
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring for Windows")
    set(LIBTORCH_PATH "${PROJECT_SOURCE_DIR}/libtorch/windows")
    set(PKMN_LIB "${PROJECT_SOURCE_DIR}/pkmn/pkmn_showdown.lib")  # Windows .lib
else()
    message(STATUS "Configuring for Linux")
    set(LIBTORCH_PATH "${PROJECT_SOURCE_DIR}/libtorch/linux")
    set(PKMN_LIB "${PROJECT_SOURCE_DIR}/pkmn/libpkmn_showdown.a")    # Linux .a
endif()

# Torch setup
set(CMAKE_PREFIX_PATH "${LIBTORCH_PATH}")
set(FPHSA_NAME_MISMATCHED ON)
set(Torch_DIR "${LIBTORCH_PATH}/share/cmake/Torch")
find_package(Torch REQUIRED)

# OpenMP
find_package(OpenMP REQUIRED)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/driver
    ${PROJECT_SOURCE_DIR}/driver/src
    ${PROJECT_SOURCE_DIR}/driver/src/gen1
    ${PROJECT_SOURCE_DIR}/driver/src/gen1/data
    ${PROJECT_SOURCE_DIR}/driver/src/gen1/samplers
    ${PROJECT_SOURCE_DIR}/pkmn
    ${LIBTORCH_PATH}
    ${PROJECT_SOURCE_DIR}/pkmnaz
)

# Compiler flags (conditionally)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O2")
elseif (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
endif()

# Libraries and targets
add_library(driver STATIC driver/pkmndriver.cpp)

add_executable(example example.cpp)
target_link_libraries(example PUBLIC driver PUBLIC ${PKMN_LIB})

add_library(pkmnaz STATIC pkmnaz/model.cpp)
target_link_libraries(pkmnaz
    PUBLIC driver
    PUBLIC ${TORCH_LIBRARIES}
    PUBLIC ${PKMN_LIB}
    PUBLIC OpenMP::OpenMP_CXX
)

add_executable(mcts_analysis mcts_test.cpp)
target_link_libraries(mcts_analysis PUBLIC driver PUBLIC ${PKMN_LIB})

add_executable(tester main.cpp)
target_link_libraries(tester PUBLIC pkmnaz)
