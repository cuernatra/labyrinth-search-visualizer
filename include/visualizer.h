#pragma once

#include <SFML/Graphics.hpp>
#include "grid.h"

class Visualizer
{
    public:
        void draw(sf::RenderWindow& window, const Grid& grid);

    private:
        float gap = 2.0f;

        sf::Color getColor(NodeState state);
};