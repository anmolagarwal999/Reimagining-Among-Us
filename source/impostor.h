#include "main.h"

#ifndef IMPOSTOR_H
#define IMPOSTOR_H

class Impostor_Obj
{
public:
    Impostor_Obj(){};

    Impostor_Obj(const int &ORTHO_LEFT_X,
                 const int &ORTHO_RIGHT_X,
                 const int &ORTHO_LEFT_Y,
                 const int &ORTHO_RIGHT_Y,
                 const int &ORTHO_LEFT_Z,
                 const int &ORTHO_RIGHT_Z, const int &NUM_COLS, const int &NUM_ROWS);

    float rotation;
    void draw(glm::mat4, const GLuint program_impostor_ID);
    double speed;
    int curr_col_num;
    int curr_row_num;
    char target_pending;
    glm::vec3 position;
    void chase_crewmate(const char &dir);
    unsigned int num_elements_in_indices;
    // std::vector<glm::vec3> displacement_positions;
    float x_length_rect, y_length_rect;
    // void set_position(const float x, const float y);

private:
    VAO *object;
};

#endif // IMPOSTOR_H
