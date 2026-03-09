#include "visualizer.h"
#include "app.h"

void Visualizer::draw(sf::RenderWindow& window, const Grid& grid, int cellSize, int marginX, int marginY)
{
    float offsetX = static_cast<float>(marginX);
    float offsetY = static_cast<float>(marginY);

    sf::RectangleShape cell;
    cell.setSize({ cellSize - gap, cellSize - gap });

    for (const auto& row : grid.getAllNodes())
    {
        for (const auto& node : row)
        {
            cell.setPosition(
                node.pos.col * cellSize + gap / 2.0f + offsetX,
                node.pos.row * cellSize + gap / 2.0f + offsetY
            );

            cell.setFillColor(getColor(node.state));
            window.draw(cell);
        }
    }
}

sf::Color Visualizer::getColor(NodeState state)
{
    switch (state)
    {
        case NodeState::Wall: return sf::Color::Black;
        case NodeState::Start: return sf::Color::Green;
        case NodeState::Goal: return sf::Color::Red;
        case NodeState::Visited: return sf::Color(100, 149, 237);
        case NodeState::Path: return sf::Color(10, 121, 124);
        case NodeState::DeadEnd: return sf::Color(200, 50, 50);
        case NodeState::Backtracked: return sf::Color(50, 50, 50);
        default: return sf::Color::White;
    }
}