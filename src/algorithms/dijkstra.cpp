#include "algorithms/dijkstra.h"

#include <algorithm>
#include <limits>

bool Dijkstra::isWalkable(NodeState state) const
{
    return state != NodeState::Wall;
}

void Dijkstra::reset(Grid& grid)
{
    frontier = {};

    running = false;
    startIndex = -1;
    goalIndex = -1;
    width = grid.getWidth();
    height = grid.getHeight();

    parent.clear();
    distance.clear();
    finalized.clear();
    lastPathLength = -1;
    lastPathCost = -1;

    for (auto& row : grid.getAllNodes())
    {
        for (auto& node : row)
        {
            if (node.state == NodeState::Visited ||
                node.state == NodeState::Backtracked ||
                node.state == NodeState::Path)
            {
                node.state = NodeState::Empty;
            }
        }
    }
}

bool Dijkstra::start(Grid& grid)
{
    reset(grid);

    width = grid.getWidth();
    height = grid.getHeight();

    if (width <= 0 || height <= 0)
        return false;

    const int nodeCount = width * height;
    const int inf = std::numeric_limits<int>::max();

    parent.assign(nodeCount, -1);
    distance.assign(nodeCount, inf);
    finalized.assign(nodeCount, false);

    int startRow = -1;
    int startCol = -1;
    int goalRow = -1;
    int goalCol = -1;

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            NodeState state = grid.getNode(row, col).state;

            if (state == NodeState::Start)
            {
                startRow = row;
                startCol = col;
            }
            else if (state == NodeState::Goal)
            {
                goalRow = row;
                goalCol = col;
            }
        }
    }

    if (startRow < 0 || goalRow < 0)
        return false;

    startIndex = toIndex(startRow, startCol);
    goalIndex = toIndex(goalRow, goalCol);

    distance[startIndex] = 0;
    frontier.push({ 0, startIndex });
    running = true;

    return true;
}

void Dijkstra::applyFinalStates(Grid& grid, int foundGoalIndex)
{
    std::vector<bool> inFinalPath(width * height, false);

    int current = foundGoalIndex;
    int pathNodeCount = 0;
    while (current != -1)
    {
        inFinalPath[current] = true;
        ++pathNodeCount;
        if (current == startIndex)
            break;
        current = parent[current];
    }

    lastPathLength = std::max(0, pathNodeCount - 1);
    lastPathCost = distance[foundGoalIndex];

    const int inf = std::numeric_limits<int>::max();

    for (int index = 0; index < width * height; ++index)
    {
        if (distance[index] == inf)
            continue;

        int row = index / width;
        int col = index % width;
        Node& node = grid.getNode(row, col);

        if (node.state == NodeState::Start || node.state == NodeState::Goal)
            continue;

        node.state = inFinalPath[index] ? NodeState::Path : NodeState::Backtracked;
    }
}

Dijkstra::StepResult Dijkstra::step(Grid& grid, int maxSteps)
{
    if (!running)
        return StepResult::InvalidMaze;

    const int stepBudget = std::max(1, maxSteps);

    for (int iteration = 0; iteration < stepBudget; ++iteration)
    {
        while (!frontier.empty())
        {
            const QueueEntry& top = frontier.top();

            if (top.distance != distance[top.index] || finalized[top.index])
            {
                frontier.pop();
                continue;
            }

            break;
        }

        if (frontier.empty())
        {
            running = false;
            return StepResult::NoPath;
        }

        QueueEntry currentEntry = frontier.top();
        frontier.pop();

        int current = currentEntry.index;
        finalized[current] = true;

        int row = current / width;
        int col = current % width;
        Node& currentNode = grid.getNode(row, col);

        if (current == goalIndex)
        {
            applyFinalStates(grid, current);
            running = false;
            return StepResult::Found;
        }

        if (currentNode.state != NodeState::Start && currentNode.state != NodeState::Goal)
            currentNode.state = NodeState::Visited;

        const int dRow[4] = { -1, 1, 0, 0 };
        const int dCol[4] = { 0, 0, -1, 1 };

        for (int i = 0; i < 4; ++i)
        {
            int nextRow = row + dRow[i];
            int nextCol = col + dCol[i];

            if (nextRow < 0 || nextRow >= height || nextCol < 0 || nextCol >= width)
                continue;

            int nextIndex = toIndex(nextRow, nextCol);
            if (finalized[nextIndex])
                continue;

            Node& nextNode = grid.getNode(nextRow, nextCol);
            if (!isWalkable(nextNode.state))
                continue;

            int candidateDistance = distance[current] + std::max(1, nextNode.weight);
            if (candidateDistance >= distance[nextIndex])
                continue;

            distance[nextIndex] = candidateDistance;
            parent[nextIndex] = current;
            frontier.push({ candidateDistance, nextIndex });
        }
    }

    return StepResult::Running;
}
