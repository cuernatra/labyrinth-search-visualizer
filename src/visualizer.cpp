#include "visualizer.h"
#include "app.h"

#include <algorithm>
#include <string>

Visualizer::Visualizer()
{
    weightFontLoaded = weightFont.loadFromFile("C:/Windows/Fonts/arial.ttf");

    if (weightFontLoaded)
    {
        weightText.setFont(weightFont);
        weightText.setFillColor(sf::Color::Black);

        for (size_t i = 0; i < weightLabels.size(); ++i)
            weightLabels[i] = std::to_string(static_cast<int>(i));
    }
}

void Visualizer::draw(sf::RenderWindow& window, const Grid& grid, int cellSize, int marginX, int marginY, bool showWeights)
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

            if (showWeights && weightFontLoaded && node.state != NodeState::Wall && cellSize >= 12)
            {
                weightText.setCharacterSize(static_cast<unsigned int>(std::max(10, cellSize / 2)));

                if (node.weight >= 0 && node.weight < static_cast<int>(weightLabels.size()))
                    weightText.setString(weightLabels[static_cast<size_t>(node.weight)]);
                else
                    weightText.setString(std::to_string(node.weight));

                const sf::FloatRect textRect = weightText.getLocalBounds();
                const float textX = node.pos.col * cellSize + offsetX + (cellSize - textRect.width) * 0.5f - textRect.left;
                const float textY = node.pos.row * cellSize + offsetY + (cellSize - textRect.height) * 0.5f - textRect.top;
                weightText.setPosition(textX, textY);
                window.draw(weightText);
            }
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