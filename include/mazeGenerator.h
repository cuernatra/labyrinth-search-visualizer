#pragma once

#include "Grid.h"

class MazeGenerator
{
    public:
        static void generateSpawnNode(Grid& grid);
        static void generateMazeBase(Grid& grid);
        bool generateMazeRoute(Grid& grid);
        bool generateRemainMaze(Grid& grid);
        bool finalizeMaze(Grid& grid);

        static int countPathNeighbors(Grid& grid, int row, int col);

        void checkNeighbor(
            Grid& grid,
            int newRow,
            int newCol,
            int rowCount,
            int colCount,
            std::vector<Node*>& neighbors,
            bool allowIfCurrentRowIsOne = false);

        const std::vector<Node*> getPathStack() const { return pathStack; }

    private:
        std::vector<Node*> pathStack;
        Node* current = nullptr;
        bool started = false;
        int routeNeighbourCount = 0;
        size_t remainSeedIndex = 0;
};