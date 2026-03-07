#pragma once

#include <vector>
#include "node.h"

struct Size
{
    int width;
    int height;
};

class Grid
{
    public:
        Size size;
        std::vector<std::vector<Node>> nodes;

        Grid(int width, int height);

        Node& getNode(int row, int col);
}; 