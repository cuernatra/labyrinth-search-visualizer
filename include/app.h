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

    void updateLayout();

    void resetApp(bool loadSelectedMaze);

private:
    void processEvents();
    void update();
    void render();

    Grid grid;
    Visualizer visualizer;
    MazeGenerator generator;

    static constexpr int defaultGridWidth = 31;
    static constexpr int defaultGridHeight = 51;

    int cellSize = 15;

    bool possibleToContinue = false;
    bool possibleToStart = true;

    int selectedMazeId = 0;
    bool selectMaze = false;

    static constexpr int marginX = 200;
    static constexpr int marginY = 20;

    static constexpr int maxGridPixels = 950;

    bool mazeRouteGenerated = false;
    bool remainMazeGenerated = false;
    bool MazeReady = false;

    bool paused = true;

    sf::RenderWindow window;
};