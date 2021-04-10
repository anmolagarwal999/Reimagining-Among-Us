#include "main.h"

#ifndef MAZE_H
#define MAZE_H

class Maze_obj
{
public:
    Maze_obj() {}
    Maze_obj(const int &ORTHO_LEFT_X,
                   const int &ORTHO_RIGHT_X,
                   const int &ORTHO_LEFT_Y,
                   const int &ORTHO_RIGHT_Y,
                   const int &ORTHO_LEFT_Z,
                   const int &ORTHO_RIGHT_Z, const int &NUM_COLS, const int &NUM_ROWS);
    void draw();
    std::vector<std::pair<float, float>> grid_tracking;
    std::vector<std::pair<int, int>> index_pairs; 
    unsigned int num_elements_in_indices;

private:
    VAO *object;
};

#endif // MAZE_H
