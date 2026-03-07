#pragma once

#include <SFML/Graphics.hpp>
#include "grid.h"

class Visualizer
{
    public:
        void draw(sf::RenderWindow& window, const Grid& grid);

    private:
        float cellSize = 20.0f;
        float gap = 2.0f;
};