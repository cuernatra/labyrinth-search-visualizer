#pragma once

#include "visualizer.h"
#include "grid.h"
#include "mazeGenerator.h"

#include <SFML/Graphics.hpp>

class App
{
public:
    App();
    void run();

    int getCellSize() const { return cellSize; }
    int getMargin() const { return margin; }

    void updateLayout();

private:
    void processEvents();
    void update();
    void render();

    Grid grid;
    Visualizer visualizer;
    MazeGenerator generator;

    static constexpr int defaultGridWidth = 21;
    static constexpr int defaultGridHeight = 31;

    int cellSize = 15;

    int selectedMazeId = 2;
    bool selectMaze = true;

    static constexpr int margin = 30;

    static constexpr int maxGridPixelWidth = 800;
    static constexpr int maxGridPixelHeight = 800;

    bool mazeRouteGenerated = false;
    bool remainMazeGenerated = false;
    bool MazeReady = false;

    sf::RenderWindow window;
};