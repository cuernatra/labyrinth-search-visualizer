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

    static constexpr int getCellSize() { return cellSize; }
    static constexpr int getMargin() { return margin; }

private:
    void processEvents();
    void update();
    void render();

    Grid grid;
    Visualizer visualizer;
    MazeGenerator generator;
    
    static constexpr int gridWidth = 31;
    static constexpr int gridHeight = 51;
    static constexpr int cellSize = 15;
    static constexpr int margin = 30;

    bool mazeRouteGenerated = false;
    
    sf::RenderWindow window;
};