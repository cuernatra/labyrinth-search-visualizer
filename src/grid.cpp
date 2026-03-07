#include "grid.h"

Grid::Grid(int width, int height) : 
    size{width, height}, 
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

const std::vector<std::vector<Node>>& Grid::getAllNodes() const
{
    return nodes;
}