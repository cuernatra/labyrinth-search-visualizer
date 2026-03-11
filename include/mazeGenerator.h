#pragma once

#include "Grid.h"
#include <vector>

class MazeGenerator
{
public:
    static void generateSpawnNode(Grid& grid);
    static void generateMazeBase(Grid& grid);
    bool generateMazeRoute(Grid& grid);
    bool generateRemainMaze(Grid& grid);
    bool finalizeMaze(Grid& grid);
    void setLoopCarveCount(int count);
    int getLoopCarveCount() const { return loopCarveCount; }

    static int countPathNeighbors(Grid& grid, int row, int col);

    void checkNeighbor(
        Grid& grid,
        int newRow,
        int newCol,
        int rowCount,
        int colCount,
        std::vector<Node*>& neighbors,
        bool allowIfCurrentRowIsOne = false);

    const std::vector<Node*>& getPathStack() const { return pathStack; }

    void startFromTheScratch(Grid& grid);

private:
    void applyRandomLoops(Grid& grid);

    std::vector<Node*> pathStack;
    Node* current = nullptr;
    bool started = false;
    int routeNeighbourCount = 0;
    size_t remainSeedIndex = 0;

    bool remainStarted = false;
    bool exploringBranch = false;
    std::vector<Node*> routeNodes;
    std::vector<Node*> expandableNodes;
    std::vector<Node*> branchStack;
    int stepsLeftInBranch = 0;
    Node* lastSeed = nullptr;

    int finalizeRow = 0;
    int finalizeCol = 0;

    int loopCarveCount = 0;
};