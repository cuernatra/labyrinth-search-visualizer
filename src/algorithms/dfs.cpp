#include "algorithms/dfs.h"

#include <algorithm>

bool Dfs::isWalkable(NodeState state) const
{
    return state != NodeState::Wall;
}

void Dfs::reset(Grid& grid)
{
    frontier.clear();

    running = false;
    startIndex = -1;
    goalIndex = -1;
    width = grid.getWidth();
    height = grid.getHeight();

    parent.clear();
    discovered.clear();

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

bool Dfs::start(Grid& grid)
{
    reset(grid);

    width = grid.getWidth();
    height = grid.getHeight();

    if (width <= 0 || height <= 0)
        return false;

    parent.assign(width * height, -1);
    discovered.assign(width * height, false);

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

    discovered[startIndex] = true;
    frontier.push_back(startIndex);
    running = true;

    return true;
}

void Dfs::applyFinalStates(Grid& grid, int foundGoalIndex)
{
    std::vector<bool> inFinalPath(width * height, false);

    int current = foundGoalIndex;
    while (current != -1)
    {
        inFinalPath[current] = true;
        if (current == startIndex)
            break;
        current = parent[current];
    }

    for (int index = 0; index < width * height; ++index)
    {
        if (!discovered[index])
            continue;

        int row = index / width;
        int col = index % width;
        Node& node = grid.getNode(row, col);

        if (node.state == NodeState::Start || node.state == NodeState::Goal)
            continue;

        node.state = inFinalPath[index] ? NodeState::Path : NodeState::Backtracked;
    }
}

Dfs::StepResult Dfs::step(Grid& grid, int maxSteps)
{
    if (!running)
        return StepResult::InvalidMaze;

    const int stepBudget = std::max(1, maxSteps);

    for (int iteration = 0; iteration < stepBudget; ++iteration)
    {
        if (frontier.empty())
        {
            running = false;
            return StepResult::NoPath;
        }

        int current = frontier.back();
        frontier.pop_back();

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
            if (discovered[nextIndex])
                continue;

            Node& nextNode = grid.getNode(nextRow, nextCol);
            if (!isWalkable(nextNode.state))
                continue;

            discovered[nextIndex] = true;
            parent[nextIndex] = current;
            frontier.push_back(nextIndex);
        }
    }

    return StepResult::Running;
}
