cmake_minimum_required(VERSION 3.20)

project(labyrinth-search-visualizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

FetchContent_MakeAvailable(SFML)

add_executable(labyrinth-search-visualizer
    src/main.cpp
    src/app.cpp
    src/grid.cpp
    src/visualizer.cpp
    src/algorithms/bfs.cpp
    src/mazeGenerator.cpp
)

target_include_directories(labyrinth-search-visualizer PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(labyrinth-search-visualizer PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)