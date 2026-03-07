#pragma once

#include "visualizer.h"
#include "grid.h"

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

    static constexpr int gridWidth = 20;
    static constexpr int gridHeight = 30;
    static constexpr int cellSize = 25;
    static constexpr int margin = 30;

    sf::RenderWindow window;
};