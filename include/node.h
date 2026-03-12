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
        int weight = 1;
};