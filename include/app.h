#pragma once

#include "visualizer.h"
#include "grid.h"

#include <SFML/Graphics.hpp>

class App
{
public:
    App();
    void run();

private:
    void processEvents();
    void update();
    void render();

    Grid grid;
    Visualizer visualizer;

    sf::RenderWindow window;
};