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
        started = true;
        return false;
    }

    current->clearNeighbors();

    int row = current->pos.row;
    int col = current->pos.col;

    if (row - 1 >= 0)
    {
        Node& up = grid.getNode(row - 1, col);

        if (up.state == NodeState::Empty || row == 1)
            current->addNeighbor(&up);
    }

    if (row + 1 < rowCount)
    {
        Node& down = grid.getNode(row + 1, col);
        if (down.state == NodeState::Empty)
            current->addNeighbor(&down);
    }

    if (col - 1 >= 0)
    {
        Node& left = grid.getNode(row, col - 1);
        if (left.state == NodeState::Empty)
            current->addNeighbor(&left);
    }

    if (col + 1 < colCount)
    {
        Node& right = grid.getNode(row, col + 1);
        if (right.state == NodeState::Empty)
            current->addNeighbor(&right);
    }

    if (current->getNeighbors().empty())
    {
        started = false;
        return false;
    }

    std::uniform_int_distribution<int> dist(
        0, current->getNeighbors().size() - 1
    );

    current = current->getNeighbors()[dist(rng)];

    if (current->pos.row == 0)
    {
        current->state = NodeState::Goal;
        return true;
    }

    if (current->state != NodeState::Start)
        current->state = NodeState::Visited;

    return false;
}