#pragma once

#include "grid.h"
#include <vector>

class MazeStorage
{
public:
    static bool saveMaze(const Grid& grid);
    static bool loadMaze(Grid& grid, int targetId);
    static std::vector<int> getAvailableMazeIds();

private:
    static int getNextMazeId();
    static char nodeStateToChar(NodeState state);
    static NodeState charToNodeState(char c);
};