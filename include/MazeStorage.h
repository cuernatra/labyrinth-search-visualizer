#pragma once

#include "grid.h"

class MazeStorage
{
public:
    static bool saveMaze(const Grid& grid);
    static bool loadMaze(Grid& grid, int targetId);

private:
    static int getNextMazeId();
    static char nodeStateToChar(NodeState state);
    static NodeState charToNodeState(char c);
};