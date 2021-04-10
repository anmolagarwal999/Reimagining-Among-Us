#include "main.h"

#ifndef PROPS_H
#define PROPS_H

class Props_Obj
{
public:
    Props_Obj(){};

    Props_Obj(const int &ORTHO_LEFT_X,
              const int &ORTHO_RIGHT_X,
              const int &ORTHO_LEFT_Y,
              const int &ORTHO_RIGHT_Y,
              const int &ORTHO_LEFT_Z,
              const int &ORTHO_RIGHT_Z, const int &NUM_COLS, const int &NUM_ROWS, const int &color_idx,
              const std::string &obj_desc);

    float rotation;
    void draw(glm::mat4, const GLuint program_impostor_ID);
    double speed;
    bool is_visible;
    glm::vec3 position;
    int curr_col_num;
    int curr_row_num;
    std::string desc;
    // void chase_crewmate(const char &dir);
    unsigned int num_elements_in_indices;
    // std::vector<glm::vec3> displacement_positions;
    float x_length_rect, y_length_rect;
    // void set_position(const float x, const float y);

private:
    VAO *object;
};

#endif // PROPS_H
