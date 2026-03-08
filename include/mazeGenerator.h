#pragma once

#include "Grid.h"

class MazeGenerator
{
    public:
        static void generateSpawnNode(Grid& grid);
        static void generateMazeBase(Grid& grid);
        bool generateMazeRoute(Grid& grid);
        bool generateRemainMaze(Grid& grid);

        static int countPathNeighbors(Grid& grid, int row, int col);

        void checkNeighbor(
            Grid& grid,
            int newRow,
            int newCol,
            int rowCount,
            int colCount,
            std::vector<Node*>& neighbors,
            bool allowIfCurrentRowIsOne = false);

    private:
        std::vector<Node*> pathStack;
        Node* current = nullptr;
        bool started = false;
        int routeNeighbourCount = 0;
};