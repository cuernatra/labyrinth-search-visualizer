#pragma once

#include "grid.h"

#include <vector>

class Dfs
{
public:
    enum class StepResult
    {
        Running,
        Found,
        NoPath,
        InvalidMaze
    };

    bool start(Grid& grid);
    StepResult step(Grid& grid, int maxSteps);
    void reset(Grid& grid);

    bool isRunning() const { return running; }

private:
    int toIndex(int row, int col) const { return row * width + col; }
    bool isWalkable(NodeState state) const;
    void applyFinalStates(Grid& grid, int goalIndex);

    int width = 0;
    int height = 0;
    int startIndex = -1;
    int goalIndex = -1;

    bool running = false;

    std::vector<int> frontier;
    std::vector<int> parent;
    std::vector<bool> discovered;
};
