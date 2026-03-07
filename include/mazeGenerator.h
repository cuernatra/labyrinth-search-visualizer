#pragma once

#include "Grid.h"

class MazeGenerator
{
    public:
        bool generateMaze(Grid& grid);
        static void generateMazeBase(Grid& grid);
        static void generateSpawnNode(Grid& grid);

    private:
        Node* current = nullptr;
        bool started = false;
};