#pragma once

#include "grid.h"

#include <queue>
#include <vector>

class Dijkstra
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
    int getLastPathLength() const { return lastPathLength; }
    int getLastPathCost() const { return lastPathCost; }

private:
    struct QueueEntry
    {
        int distance = 0;
        int index = -1;

        bool operator>(const QueueEntry& other) const
        {
            return distance > other.distance;
        }
    };

    int toIndex(int row, int col) const { return row * width + col; }
    bool isWalkable(NodeState state) const;
    void applyFinalStates(Grid& grid, int foundGoalIndex);

    int width = 0;
    int height = 0;
    int startIndex = -1;
    int goalIndex = -1;

    bool running = false;

    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> frontier;
    std::vector<int> parent;
    std::vector<int> distance;
    std::vector<bool> finalized;
    int lastPathLength = -1;
    int lastPathCost = -1;
};
