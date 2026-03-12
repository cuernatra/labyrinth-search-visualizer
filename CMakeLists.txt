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

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.7
)

FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v2.6
)

FetchContent_MakeAvailable(SFML imgui)

set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_DIR ${imgui_SOURCE_DIR})

FetchContent_MakeAvailable(imgui-sfml)

add_executable(labyrinth-search-visualizer
    src/main.cpp
    src/app.cpp
    src/grid.cpp
    src/visualizer.cpp
    src/algorithms/bfs.cpp
    src/algorithms/dfs.cpp
    src/algorithms/dijkstra.cpp
    src/mazeGenerator.cpp
    src/mazeStorage.cpp
)

target_include_directories(labyrinth-search-visualizer PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(labyrinth-search-visualizer PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    ImGui-SFML::ImGui-SFML
)

if (WIN32)
    add_custom_command(TARGET labyrinth-search-visualizer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:labyrinth-search-visualizer>
            $<TARGET_FILE_DIR:labyrinth-search-visualizer>
        COMMAND_EXPAND_LISTS
    )
endif()