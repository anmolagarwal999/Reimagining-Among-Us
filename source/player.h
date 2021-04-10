#include "main.h"

#ifndef PLAYER_H
#define PLAYER_H

class Player_Obj
{
public:
    Player_Obj() {};
    Player_Obj(const float &start_square_center_x, const float & start_square_center_y);
    glm::vec3 position;
    float rotation;
    glm::vec2 draw(glm::mat4,const GLuint program_player_ID);
    double speed;
    unsigned int num_elements_in_indices;
    std::vector<glm::vec3> displacement_positions;
    float x_length_rect,y_length_rect;
    void set_position(const float x, const float y);


private:
    VAO *object;
};

#endif // PLAYER_H
