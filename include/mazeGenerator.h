#pragma once

#include "Grid.h"

class MazeGenerator
{
    public:
        bool generateMaze(Grid& grid);
        static void generateMazeBase(Grid& grid);
        static void generateSpawnNode(Grid& grid);
        static int countPathNeighbors(Grid& grid, int row, int col);

    private:
        std::vector<Node*> pathStack;
        Node* current = nullptr;
        bool started = false;
        int routeNeighbourCount = 0;
};