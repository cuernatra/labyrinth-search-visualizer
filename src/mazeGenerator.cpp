#include "mazeGenerator.h"
#include <random>

void MazeGenerator::generateMazeBase(Grid& grid) {
    for (auto& row : grid.getAllNodes()) {
        for (auto& node : row) {
            if (node.pos.row == 0 || node.pos.row == grid.getAllNodes().size() - 1 ||
                node.pos.col == 0 || node.pos.col == grid.getAllNodes()[0].size() - 1)
            {
                node.state = NodeState::Wall;
            }

            if (node.pos.row % 2 == 0 && node.pos.col % 2 == 0) {
                node.state = NodeState::Wall;
            }
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    int width = grid.getAllNodes()[0].size();
    std::uniform_int_distribution<> dist(0, width / 2 - 1);

    int col = dist(gen) * 2 + 1;

    int row = grid.getAllNodes().size() - 1;

    grid.setStartNode(col);

    printf("Start Node: (%d, %d)\n", grid.getStartNode().pos.row, grid.getStartNode().pos.col);
}

bool MazeGenerator::generateMaze(Grid& grid)
{
    return false;
}
