#include "mazeGenerator.h"
#include <random>
#include <algorithm>

void MazeGenerator::generateMazeBase(Grid& grid) 
{
    auto& nodes = grid.getAllNodes();
    const int rowCount = (int)nodes.size();
    const int colCount = (int)nodes[0].size();

    for (auto& row : nodes)
    {
        for (auto& node : row)
        {
            if (node.pos.row == 0 || node.pos.row == rowCount - 1 ||
                node.pos.col == 0 || node.pos.col == colCount - 1)
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

    auto& nodes = grid.getAllNodes();
    const int width = (int)nodes[0].size();

    std::uniform_int_distribution<> dist(0, width / 2 - 1);
    const int col = dist(gen) * 2 + 1;

    grid.setStartNode(col);

    printf("Start Node: (%d, %d)\n",
        grid.getStartNode().pos.row,
        grid.getStartNode().pos.col);
}

void MazeGenerator::checkNeighbor(
    Grid& grid,
    int newRow,
    int newCol,
    int rowCount,
    int colCount,
    std::vector<Node*>& neighbors,
    bool allowIfCurrentRowIsOne)
{
    if (newRow < 0 || newRow >= rowCount || newCol < 0 || newCol >= colCount)
        return;

    Node& node = grid.getNode(newRow, newCol);

    if (node.state == NodeState::Empty || allowIfCurrentRowIsOne)
        neighbors.push_back(&node);
    else if (node.state == NodeState::Visited)
        routeNeighbourCount++;
}

bool MazeGenerator::generateMazeRoute(Grid& grid)
{
    static std::mt19937 rng(std::random_device{}());

    auto& nodes = grid.getAllNodes();
    int rowCount = nodes.size();
    int colCount = nodes[0].size();

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

    Node* neighbors[4];
    int neighborCount = 0;

    auto tryAddNeighbor = [&](int newRow, int newCol, bool allowIfCurrentRowIsOne = false)
    {
        if (newRow < 0 || newRow >= rowCount || newCol < 0 || newCol >= colCount)
            return;

        Node& node = grid.getNode(newRow, newCol);

        if (node.state == NodeState::Empty || allowIfCurrentRowIsOne)
        {
            neighbors[neighborCount++] = &node;
        }
        else if (node.state == NodeState::Visited)
        {
            routeNeighbourCount++;
        }
    };

    // Up, Down, Left, Right
    tryAddNeighbor(row - 1, col, row == 1);
    tryAddNeighbor(row + 1, col);
    tryAddNeighbor(row, col - 1);
    tryAddNeighbor(row, col + 1);

    if (neighborCount > 0)
    {
        pathStack.push_back(current);

        std::uniform_int_distribution<int> dist(0, neighborCount - 1);
        current = neighbors[dist(rng)];

        if (current->pos.row == 0 && current->state == NodeState::Wall)
        {
            current->state = NodeState::Goal;
            started = false;
            return true;
        }

        if (current->state != NodeState::Start)
            current->state = NodeState::Visited;

        return false;
    }

    if (current->state != NodeState::Start)
    {
        if (routeNeighbourCount >= 2)
            current->state = NodeState::DeadEnd;
        else
            current->state = NodeState::Backtracked;

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

bool MazeGenerator::generateRemainMaze(Grid& grid)
{
    static std::mt19937 rng(std::random_device{}());

    static bool remainStarted = false;
    static bool exploringBranch = false;

    static std::vector<Node*> routeNodes;
    static std::vector<Node*> expandableNodes;
    static std::vector<Node*> branchStack;

    static int stepsLeftInBranch = 0;
    static Node* lastSeed = nullptr;

    int rowCount = grid.getAllNodes().size();
    int colCount = grid.getAllNodes()[0].size();

    auto isPath = [](NodeState state) -> bool
    {
        return state == NodeState::Start ||
               state == NodeState::Goal ||
               state == NodeState::Visited ||
               state == NodeState::Backtracked ||
               state == NodeState::DeadEnd;
    };

    auto countPathNeighborsLocal = [&](int row, int col) -> int
    {
        int count = 0;

        if (row - 1 >= 0 && isPath(grid.getNode(row - 1, col).state)) count++;
        if (row + 1 < rowCount && isPath(grid.getNode(row + 1, col).state)) count++;
        if (col - 1 >= 0 && isPath(grid.getNode(row, col - 1).state)) count++;
        if (col + 1 < colCount && isPath(grid.getNode(row, col + 1).state)) count++;

        return count;
    };

    auto collectValidNeighbors = [&](Node* node) -> std::vector<Node*>
    {
        std::vector<Node*> neighbors;
        int row = node->pos.row;
        int col = node->pos.col;

        auto tryAdd = [&](int nr, int nc)
        {
            if (nr < 0 || nr >= rowCount || nc < 0 || nc >= colCount)
                return;

            Node& candidate = grid.getNode(nr, nc);

            if (candidate.state != NodeState::Empty)
                return;

            if (countPathNeighborsLocal(nr, nc) <= 1)
                neighbors.push_back(&candidate);
            else candidate.state = NodeState::DeadEnd;
        };

        tryAdd(row - 1, col);
        tryAdd(row + 1, col);
        tryAdd(row, col - 1);
        tryAdd(row, col + 1);

        return neighbors;
    };

    auto containsNode = [](const std::vector<Node*>& vec, Node* node) -> bool
    {
        for (Node* n : vec)
        {
            if (n == node)
                return true;
        }
        return false;
    };

    auto removeNodesWithoutNeighbors = [&](std::vector<Node*>& vec)
    {
        vec.erase(
            std::remove_if(vec.begin(), vec.end(),
                [&](Node* node)
                {
                    return node == nullptr || collectValidNeighbors(node).empty();
                }),
            vec.end());
    };

    if (!remainStarted)
    {
        routeNodes = getPathStack();

        if (!routeNodes.empty())
            routeNodes.pop_back();

        else return true;

        expandableNodes.clear();
        branchStack.clear();
        exploringBranch = false;
        stepsLeftInBranch = 0;
        lastSeed = nullptr;

        remainStarted = true;
        return false;
    }

    removeNodesWithoutNeighbors(routeNodes);
    removeNodesWithoutNeighbors(expandableNodes);

    if (!exploringBranch)
    {
        std::vector<Node*> possibleSeeds;

        for (Node* node : routeNodes)
        {
            if (!containsNode(possibleSeeds, node))
                possibleSeeds.push_back(node);
        }

        for (Node* node : expandableNodes)
        {
            if (!containsNode(possibleSeeds, node))
            {
                possibleSeeds.push_back(node);
            }

        }

        if (possibleSeeds.empty())
        {
            remainStarted = false;
            branchStack.clear();
            return true;
        }

        if (possibleSeeds.size() > 1 && lastSeed != nullptr)
        {
            std::vector<Node*> filtered;
            for (Node* seed : possibleSeeds)
            {
                if (seed != lastSeed)
                    filtered.push_back(seed);
            }

            if (!filtered.empty())
                possibleSeeds = filtered;
        }

        std::uniform_int_distribution<int> seedDist(0, (int)possibleSeeds.size() - 1);
        current = possibleSeeds[seedDist(rng)];
        lastSeed = current;

        branchStack.clear();
        exploringBranch = true;

        int maxSteps = std::max(1, (rowCount * colCount) / 20);
        std::uniform_int_distribution<int> stepDist(1, maxSteps);
        stepsLeftInBranch = stepDist(rng);

        return false;
    }

    if (stepsLeftInBranch <= 0)
    {
        exploringBranch = false;
        branchStack.clear();
        return false;
    }

    std::vector<Node*> neighbors = collectValidNeighbors(current);

    if (!neighbors.empty())
    {
        branchStack.push_back(current);

        std::uniform_int_distribution<int> dist(0, (int)neighbors.size() - 1);
        current = neighbors[dist(rng)];

        if (current->state != NodeState::Start && current->state != NodeState::Goal)
            current->state = NodeState::Visited;

        if (!containsNode(expandableNodes, current))
            expandableNodes.push_back(current);

        stepsLeftInBranch--;
        return false;
    }

    if (!branchStack.empty())
    {
        current = branchStack.back();
        branchStack.pop_back();
        stepsLeftInBranch--;
        return false;
    }

    exploringBranch = false;
    stepsLeftInBranch = 0;
    return false;
}

bool MazeGenerator::finalizeMaze(Grid& grid)
{
    static int row = 0;
    static int col = 0;

    auto& nodes = grid.getAllNodes();

    int rowCount = nodes.size();
    int colCount = nodes[0].size();

    if (row >= rowCount)
    {
        row = 0;
        col = 0;
        return true;
    }

    Node& node = nodes[row][col];

    if (node.state == NodeState::DeadEnd ||
        node.state == NodeState::Empty) 
    {
        node.state = NodeState::Wall;
    }

    else if (node.state == NodeState::Visited || 
        node.state == NodeState::Backtracked)
    {
        node.state = NodeState::Path;
    }

    col++;

    if (col >= colCount)
    {
        col = 0;
        row++;
    }

    return false;
}