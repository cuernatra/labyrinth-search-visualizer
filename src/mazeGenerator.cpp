#include "mazeGenerator.h"

void MazeGenerator::generateMaze(Grid& grid) {
    for (auto& row : grid.getAllNodes()) {
        for (auto& node : row) {
            if (node.pos.row == 0 || node.pos.row == grid.getAllNodes().size() - 1 ||
                node.pos.col == 0 || node.pos.col == grid.getAllNodes()[0].size() - 1)
            {
                node.state = NodeState::Wall;
            }
        }
    }
}