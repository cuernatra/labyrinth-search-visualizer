#include "visualizer.h"

void Visualizer::draw(sf::RenderWindow& window, const Grid& grid) 
{
    for (const auto& row : grid.getAllNodes())
    {
        for (const auto& node : row)
        {
            sf::RectangleShape cell;
    
            cell.setSize({20 - gap, 20 - gap});
            cell.setPosition(
                node.pos.col * cellSize + gap / 2.0f,
                node.pos.row * cellSize + gap / 2.0f
            );
            cell.setFillColor(sf::Color::White);

            window.draw(cell);
        }
    }
}