#include "mazeStorage.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

char MazeStorage::nodeStateToChar(NodeState state)
{
    switch (state)
    {
        case NodeState::Empty: return 'e';
        case NodeState::Wall: return 'w';
        case NodeState::Start: return 's';
        case NodeState::Goal: return 'g';
        case NodeState::Visited: return 'v';
        case NodeState::Path: return 'p';
        case NodeState::DeadEnd: return 'd';
        case NodeState::Backtracked: return 'b';
        default: return '?';
    }
}

NodeState MazeStorage::charToNodeState(char c)
{
    switch (c)
    {
        case 'e': return NodeState::Empty;
        case 'w': return NodeState::Wall;
        case 's': return NodeState::Start;
        case 'g': return NodeState::Goal;
        case 'v': return NodeState::Visited;
        case 'p': return NodeState::Path;
        case 'd': return NodeState::DeadEnd;
        case 'b': return NodeState::Backtracked;
        default: return NodeState::Empty;
    }
}

int MazeStorage::getNextMazeId()
{
    std::ifstream file("mazes.txt");

    if (!file.is_open())
        return 0;

    std::string line;
    int maxId = -1;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;

        if (std::getline(ss, token, ';'))
        {
            int id = std::stoi(token);
            if (id > maxId)
                maxId = id;
        }
    }

    return maxId + 1;
}

bool MazeStorage::saveMaze(const Grid& grid)
{
    int rows = grid.getHeight();
    int cols = grid.getWidth();
    int id = getNextMazeId();

    std::ofstream file("mazes.txt", std::ios::app);

    if (!file.is_open())
        return false;

    file << id << ";" << rows << ";" << cols;

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            file << ";" << nodeStateToChar(grid.getNode(row, col).state);
        }
    }

    file << "\n";

    return true;
}

bool MazeStorage::loadMaze(Grid& grid, int targetId)
{
    std::ifstream file("mazes.txt");

    if (!file.is_open())
        return false;

    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(ss, token, ';'))
            tokens.push_back(token);

        if (tokens.size() < 3)
            continue;

        int id = std::stoi(tokens[0]);

        if (id != targetId)
            continue;

        int rows = std::stoi(tokens[1]);
        int cols = std::stoi(tokens[2]);

        if ((int)tokens.size() != 3 + rows * cols)
            return false;

        grid.resize(cols, rows);

        int index = 3;

        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < cols; col++)
            {
                NodeState state = charToNodeState(tokens[index][0]);
                grid.getNode(row, col).state = state;

                if (state == NodeState::Start)
                    grid.setStartNode(row, col);

                index++;
            }
        }

        return true;
    }

    return false;
}

std::vector<int> MazeStorage::getAvailableMazeIds()
{
    std::vector<int> ids;

    std::ifstream file("mazes.txt");
    if (!file.is_open())
        return ids;

    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;

        if (std::getline(ss, token, ';'))
        {
            ids.push_back(std::stoi(token));
        }
    }

    return ids;
}