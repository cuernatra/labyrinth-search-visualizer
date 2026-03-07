#pragma once

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
    Path
};

class Node
{
    public:
        Pos pos;
        NodeState state = NodeState::Empty;
};