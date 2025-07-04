cmake_minimum_required(VERSION 3.16)
project(GameOfLife LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(MPI REQUIRED)

find_package(Eigen3 REQUIRED)

include(FetchContent)
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG        3.0.1
  GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(SFML)

add_executable(game_of_life
    src/main.cpp
    src/GameOfLife.cpp
    src/GameOfLifeUI.cpp
    src/LocalGameOfLife.cpp
)

target_include_directories(game_of_life PRIVATE
    ${MPI_INCLUDE_PATH}
    ${EIGEN3_INCLUDE_DIR}
    src
)

target_link_libraries(game_of_life
    PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    MPI::MPI_CXX
)
