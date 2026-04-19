#include "visualizer.h"
#include "app.h"

#include <algorithm>
#include <string>
#include <vector>

Visualizer::Visualizer()
{
    const std::vector<std::string> fontCandidates =
    {
        // macOS
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf",

        // Windows
        "C:/Windows/Fonts/arial.ttf",

        // Linux
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf"
    };

    for (const std::string& fontPath : fontCandidates)
    {
        if (weightFont.loadFromFile(fontPath))
        {
            weightFontLoaded = true;
            break;
        }
    }

    if (weightFontLoaded)
    {
        weightText.setFont(weightFont);

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

            if (showWeights && weightFontLoaded && node.state != NodeState::Wall && cellSize >= 8)
            {
                const unsigned int characterSize = static_cast<unsigned int>(std::clamp(cellSize - 2, 8, 28));
                weightText.setCharacterSize(characterSize);

                if (node.weight >= 0 && node.weight < static_cast<int>(weightLabels.size()))
                    weightText.setString(weightLabels[static_cast<size_t>(node.weight)]);
                else
                    weightText.setString(std::to_string(node.weight));

                const sf::Color cellColor = getColor(node.state);
                const int brightness = static_cast<int>(cellColor.r) + static_cast<int>(cellColor.g) + static_cast<int>(cellColor.b);
                const bool darkCell = brightness < (128 * 3);

                weightText.setFillColor(darkCell ? sf::Color::White : sf::Color::Black);
                weightText.setOutlineThickness(cellSize >= 16 ? 1.f : 0.f);
                weightText.setOutlineColor(darkCell ? sf::Color::Black : sf::Color::White);

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