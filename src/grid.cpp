#include "grid.h"
#include <stdexcept>

Grid::Grid(int width, int height) 
    : size{width, height}, 
        nodes(height, std::vector<Node>(width))
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            nodes[i][j].pos = {i, j};
        }
    }
}

Node& Grid::getNode(int row, int col)
{
    return nodes[row][col];
}

std::vector<std::vector<Node>>& Grid::getAllNodes() 
{
    return nodes;
}

const std::vector<std::vector<Node>>& Grid::getAllNodes() const 
{
    return nodes;
}

void Grid::setStartNode(int col) {
    if (!nodes.empty() && !nodes[0].empty()) 
    {
        nodes[nodes.size() - 1][col].state = NodeState::Start;
        startNode = &nodes[nodes.size() - 1][col];
    }
}

Node& Grid::getStartNode() {
    return *startNode;
}

void Grid::resetMaze()
{
    for (auto& row : getAllNodes())
    {
        for (auto& node : row)
        {
            if (node.state == NodeState::Visited ||
                node.state == NodeState::Goal ||
                node.state == NodeState::Path)
            {
                node.state = NodeState::Empty;
            }
        }
    }

    getStartNode().state = NodeState::Start;
}