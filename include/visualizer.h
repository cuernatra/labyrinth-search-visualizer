#pragma once

#include <SFML/Graphics.hpp>
#include "grid.h"

class Visualizer
{
    public:
        void draw(sf::RenderWindow& window, const Grid& grid, int cellSize, int marginX, int marginY);

    private:
        float gap = 1.f;

        sf::Color getColor(NodeState state);
};