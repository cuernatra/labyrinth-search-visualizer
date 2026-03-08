#include "mazeGenerator.h"
#include <random>

void MazeGenerator::generateMazeBase(Grid& grid) 
{
    for (auto& row : grid.getAllNodes()) {
        for (auto& node : row) {
            if (node.pos.row == 0 || node.pos.row == grid.getAllNodes().size() - 1 ||
                node.pos.col == 0 || node.pos.col == grid.getAllNodes()[0].size() - 1)
            {
                node.state = NodeState::Wall;
            }

            if (node.pos.row % 2 == 0 && node.pos.col % 2 == 0) 
            {
                node.state = NodeState::Wall;
            }
        }
    }
    generateSpawnNode(grid);
}

void MazeGenerator::generateSpawnNode(Grid& grid) 
{
    std::random_device rd;
    std::mt19937 gen(rd());

    int width = grid.getAllNodes()[0].size();
    std::uniform_int_distribution<> dist(0, width / 2 - 1);

    int col = dist(gen) * 2 + 1;

    int row = grid.getAllNodes().size() - 1;

    grid.setStartNode(col);

    printf("Start Node: (%d, %d)\n", 
        grid.getStartNode().pos.row, 
        grid.getStartNode().pos.col);
}

bool MazeGenerator::generateMaze(Grid& grid)
{
    static std::mt19937 rng(std::random_device{}());

    int rowCount = grid.getAllNodes().size();
    int colCount = grid.getAllNodes()[0].size();

    if (!started)
    {
        grid.resetMaze();
        current = &grid.getStartNode();
        pathStack.clear();
        started = true;
        return false;
    }

    int row = current->pos.row;
    int col = current->pos.col;

    std::vector<Node*> neighbors;

    if (row - 1 >= 0)
    {
        Node& up = grid.getNode(row - 1, col);

        if (up.state == NodeState::Empty || row == 1)
            neighbors.push_back(&up);
        else if (up.state == NodeState::Visited)
            routeNeighbourCount++;
    }

    if (row + 1 < rowCount)
    {
        Node& down = grid.getNode(row + 1, col);
        if (down.state == NodeState::Empty)
            neighbors.push_back(&down);
        else if (down.state == NodeState::Visited)
            routeNeighbourCount++;
    }

    if (col - 1 >= 0)
    {
        Node& left = grid.getNode(row, col - 1);
        if (left.state == NodeState::Empty)
            neighbors.push_back(&left);
        else if (left.state == NodeState::Visited)
            routeNeighbourCount++;
    }

    if (col + 1 < colCount)
    {
        Node& right = grid.getNode(row, col + 1);
        if (right.state == NodeState::Empty)
            neighbors.push_back(&right);
        else if (right.state == NodeState::Visited)
            routeNeighbourCount++;
    }

    // Neighbors found
    if (!neighbors.empty())
    {
        pathStack.push_back(current);

        std::uniform_int_distribution<int> dist(0, (int)neighbors.size() - 1);
        current = neighbors[dist(rng)];

        if (current->pos.row == 0)
        {
            current->state = NodeState::Goal;
            started = false;
            return true;
        }

        if (current->state != NodeState::Start)
            current->state = NodeState::Visited;

        return false;
    }

    // Neighbors not found
    if (current->state != NodeState::Start)
    {
        if (routeNeighbourCount >= 2)
            current->state = NodeState::DeadEnd;
        else current->state = NodeState::Backtracked;

        routeNeighbourCount = 0;
    }

    if (!pathStack.empty())
    {
        current = pathStack.back();
        pathStack.pop_back();
        return false;
    }

    started = false;
    return false;
}