#include "visualizer.h"
#include "app.h"

void Visualizer::draw(sf::RenderWindow& window, const Grid& grid) 
{
    int cellSize = App::getCellSize();
    int margin = App::getMargin();

    for (const auto& row : grid.getAllNodes())
    {
        for (const auto& node : row)
        {
            sf::RectangleShape cell;
    
            cell.setSize({cellSize - gap, cellSize - gap});
            cell.setPosition(
                node.pos.col * cellSize + gap / 2.0f + margin,
                node.pos.row * cellSize + gap / 2.0f + margin
            );
            cell.setFillColor(getColor(node.state));

            window.draw(cell);
        }
    }
}

sf::Color Visualizer::getColor(NodeState state)
{
    switch(state)
    {
        case NodeState::Wall: return sf::Color::Black;
        case NodeState::Start: return sf::Color::Green;
        case NodeState::Goal: return sf::Color::Red;
        case NodeState::Visited: return sf::Color::Color(100,149,237);
        case NodeState::Path: return sf::Color::Yellow;
        default: return sf::Color::White;
    }
}