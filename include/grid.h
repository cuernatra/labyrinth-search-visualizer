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
    const Node& getNode(int row, int col) const;

    std::vector<std::vector<Node>>& getAllNodes();
    const std::vector<std::vector<Node>>& getAllNodes() const;

    void setStartNode(int col);
    void setStartNode(int row, int col);

    Node& getStartNode();
    const Node& getStartNode() const;

    void resetMaze();

    void resize(int width, int height);

    int getWidth() const;
    int getHeight() const;
    Size getSize() const;

private:
    Size size;
    std::vector<std::vector<Node>> nodes;
    Node* startNode = nullptr;
};