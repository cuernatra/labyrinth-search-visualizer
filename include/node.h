#pragma once

struct Pos
{
    int row;
    int col;
};


class Node
{
    public:
        Pos pos;
        
        bool wall = false;
        bool visited = false;
        bool path = false;
        bool start = false;
        bool goal = false;
};