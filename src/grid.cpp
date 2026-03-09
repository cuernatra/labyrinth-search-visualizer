#include "grid.h"

Grid::Grid(int width, int height)
{
    size.width = width;
    size.height = height;

    nodes.resize(height);

    for (int row = 0; row < height; row++)
    {
        nodes[row].resize(width);

        for (int col = 0; col < width; col++)
        {
            nodes[row][col].pos.row = row;
            nodes[row][col].pos.col = col;
            nodes[row][col].state = NodeState::Empty;
        }
    }
}

Node& Grid::getNode(int row, int col)
{
    return nodes[row][col];
}

const Node& Grid::getNode(int row, int col) const
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

void Grid::setStartNode(int col)
{
    startNode = &nodes[size.height - 1][col];
    startNode->state = NodeState::Start;
}

void Grid::setStartNode(int row, int col)
{
    startNode = &nodes[row][col];
    startNode->state = NodeState::Start;
}

Node& Grid::getStartNode()
{
    return *startNode;
}

const Node& Grid::getStartNode() const
{
    return *startNode;
}

void Grid::resetMaze()
{
    for (int row = 0; row < size.height; row++)
    {
        for (int col = 0; col < size.width; col++)
        {
            if (nodes[row][col].state != NodeState::Wall &&
                nodes[row][col].state != NodeState::Start)
            {
                nodes[row][col].state = NodeState::Empty;
            }
        }
    }
}

int Grid::getWidth() const
{
    return size.width;
}

int Grid::getHeight() const
{
    return size.height;
}

Size Grid::getSize() const
{
    return size;
}

void Grid::resize(int width, int height)
{
    size.width = width;
    size.height = height;

    startNode = nullptr;

    nodes.clear();
    nodes.resize(height);

    for (int row = 0; row < height; row++)
    {
        nodes[row].resize(width);

        for (int col = 0; col < width; col++)
        {
            nodes[row][col].pos.row = row;
            nodes[row][col].pos.col = col;
            nodes[row][col].state = NodeState::Empty;
        }
    }
}