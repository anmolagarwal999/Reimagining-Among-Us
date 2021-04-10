#include "main.h"

#ifndef BALL_H
#define BALL_H

class Geometric_Obj
{
public:
    Geometric_Obj() {}
    Geometric_Obj(float x, float y, color_t color, int choice);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    double speed;
    unsigned int num_elements_in_indices;
    std::vector<glm::vec3> displacement_positions;

private:
    VAO *object;
};

#endif // BALL_H
