#pragma once

#include "Grid.h"

class MazeGenerator
{
    public:
        bool generateMaze(Grid& grid);
        static void generateMazeBase(Grid& grid);

    private:
        std::size_t currentRow = 0;
        std::size_t currentCol = 0;
};