cmake_minimum_required(VERSION 3.16)

project(GameOfLifeMPI LANGUAGES CXX)

# Require C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings (compiler dependent)
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Find MPI package
find_package(MPI REQUIRED)

# Include MPI headers
include_directories(${MPI_CXX_INCLUDE_PATH})

# Source files
set(SOURCES
    main.cpp
    LocalGameOfLife.cpp
    Logger.cpp
    GameOfLifeUI.cpp
    # add other source files here
)

# Create executable
add_executable(GameOfLife ${SOURCES})

# Link MPI libraries
target_link_libraries(GameOfLife PUBLIC MPI::MPI_CXX)

# Optionally add definitions or compile options for MPI
target_compile_definitions(GameOfLife PRIVATE USE_MPI)

# Installation rules (optional)
install(TARGETS GameOfLife
        RUNTIME DESTINATION bin)

# Enable testing (optional)
enable_testing()
# add_test(NAME RunGameOfLife COMMAND GameOfLife -n -i 10)


