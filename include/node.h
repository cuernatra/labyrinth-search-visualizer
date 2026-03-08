#pragma once

#include <vector>

struct Pos
{
    int row;
    int col;
};

enum class NodeState
{
    Empty,
    Wall,
    Start,
    Goal,
    Visited,
    Path,
    DeadEnd,
    Backtracked
};

class Node
{
    public:
        Pos pos;
        NodeState state = NodeState::Empty;

        void addNeighbor(Node* neighbor) 
        { 
            neighbors.push_back(neighbor); 
        }

        const std::vector<Node*>& getNeighbors() const 
        { 
            return neighbors; 
        }

        void clearNeighbors()
        {
            neighbors.clear();
        }
    
    private:
        bool visited = false;
        std::vector<Node*> neighbors;
};