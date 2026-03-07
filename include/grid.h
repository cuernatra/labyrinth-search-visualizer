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
        Grid(int width, int height);
        
        Node& getNode(int row, int col);
        std::vector<std::vector<Node>>& getAllNodes();
        const std::vector<std::vector<Node>>& getAllNodes() const;
        void setStartNode(int col);
        Node& getStartNode();

    private:
        Size size;
        std::vector<std::vector<Node>> nodes;
        Node startNode;
}; 