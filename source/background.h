#include "main.h"

#ifndef Background_H
#define Background_H

class Background_Obj
{
public:
    Background_Obj() {};
    Background_Obj(const float &start_square_center_x, const float & start_square_center_y);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4,const GLuint program_Background_ID);
    unsigned int num_elements_in_indices;
    std::vector<glm::vec3> displacement_positions;
    float x_length_rect,y_length_rect;
    void set_position(const float x, const float y);


private:
    VAO *object;
};

#endif // Background_H
