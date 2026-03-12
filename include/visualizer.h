#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include "grid.h"

class Visualizer
{
    public:
        Visualizer();
        void draw(sf::RenderWindow& window, const Grid& grid, int cellSize, int marginX, int marginY, bool showWeights);

    private:
        float gap = 1.f;
        sf::Font weightFont;
        bool weightFontLoaded = false;
        sf::Text weightText;
        std::array<sf::String, 21> weightLabels;

        sf::Color getColor(NodeState state);
};