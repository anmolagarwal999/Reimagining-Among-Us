#include "player.h"
#include "main.h"
#include <glm/gtx/string_cast.hpp>

#define GLM_ENABLE_EXPERIMENTAL
using namespace std;

struct VAO *create_player_vao(const GLenum primitive_mode, const int numVertices,
                              const GLfloat *vertex_buffer_data,
                              GLenum fill_mode, const vector<tuple<int, int, int>> &triangle_groups);

Player_Obj::Player_Obj(const float &start_square_center_x, const float &start_square_center_y)
{
    int i, j, k, t, tmp;
    //in vertex array, make 2 rectangles in top of each other
    debug(start_square_center_x);
    debug(start_square_center_y);
    float middle_x_c = 0.0f, middle_y_c = 0.0f, middle_z_c = 0.0f;
    this->position = glm::vec3(start_square_center_x, start_square_center_y, middle_z_c);

    this->x_length_rect = 2.0f;
    this->y_length_rect =2.0f;

    // debug(start_square_center_x);
    // debug(start_square_center_y);
    // float middle_x_c = start_square_center_x, middle_y_c = start_square_center_y, middle_z_c = 0.0f;
    // this->position = glm::vec3(middle_x_c, middle_y_c, middle_z_c);

    // float x_length_rect = 2.5f;
    // float y_length_rect = 2.0f;

    vector<pair<float, float>> rect_coordinates;
    rect_coordinates.pb({middle_x_c + this->x_length_rect / 2, middle_y_c + this->y_length_rect / 2});
    rect_coordinates.pb({middle_x_c + this->x_length_rect / 2, middle_y_c - this->y_length_rect / 2});
    rect_coordinates.pb({middle_x_c - this->x_length_rect / 2, middle_y_c - this->y_length_rect / 2});
    rect_coordinates.pb({middle_x_c - this->x_length_rect / 2, middle_y_c + this->y_length_rect / 2});

    int tot_nodes = 4;
    int num_features_per_coordinate = 6; //3 for pos, 3 for color

    //  int i, j, k, t, tmp;
    // //in vertex array, make 2 rectangles in top of each other

    // float middle_x_c = 0.0f, middle_y_c = 0.0f, middle_z_c = 0.0f;
    // this->position = glm::vec3(middle_x_c, middle_y_c, middle_z_c);

    // float x_length_rect = 1.0f;
    // float y_length_rect = 1.0f;

    // vector<pair<int, int>> rect_coordinates;
    // rect_coordinates.pb({middle_x_c + x_length_rect, middle_y_c + y_length_rect});
    // rect_coordinates.pb({middle_x_c + x_length_rect, middle_y_c - y_length_rect});
    // rect_coordinates.pb({middle_x_c - x_length_rect, middle_y_c - y_length_rect});
    // rect_coordinates.pb({middle_x_c - x_length_rect, middle_y_c + y_length_rect});

    // int tot_nodes = 4;
    // int num_features_per_coordinate = 6; //3 for pos, 3 for color

    float cyan_r = (105.0 / 255.0), cyan_g = (255.0 / 255.0), cyan_b = (255.0 / 255.0);
    float grid_points[tot_nodes * num_features_per_coordinate];
    float x_c[]={1.0f, 1.0f, 0.0f, 0.0f};
    float y_c[]={1.0f, 0.0f, 0.0f, 1.0f};
      /*
    3-----0
    |     |
    2-----1

    */
    for (i = 0; i < tot_nodes; i++)
    {
        debug(tot_nodes);
        grid_points[num_features_per_coordinate * i + 0] = rect_coordinates[i].first;
        grid_points[num_features_per_coordinate * i + 1] = rect_coordinates[i].second;
        grid_points[num_features_per_coordinate * i + 2] = middle_z_c;
        // grid_points[num_features_per_coordinate * i + 3] = cyan_r;
        // grid_points[num_features_per_coordinate * i + 4] = cyan_g;
        // grid_points[num_features_per_coordinate * i + 5] = cyan_b;
        int idx=(i+0)%4;
        grid_points[num_features_per_coordinate * i + 3] =x_c[idx];
        grid_points[num_features_per_coordinate * i + 4] =y_c[idx];
        grid_points[num_features_per_coordinate * i + 5] = 0.0f;
    }

    for (int i = 0; i < tot_nodes; i++)
    {
        cout << i << " -> "
             << "Vertex player is " << grid_points[num_features_per_coordinate * i] << " : " << grid_points[num_features_per_coordinate * i + 1] << " : " << grid_points[num_features_per_coordinate * i + 2] << " : " << grid_points[num_features_per_coordinate * i + 3] << " : " << grid_points[num_features_per_coordinate * i + 4] << " : " << grid_points[num_features_per_coordinate * i + 5] << " : " << endl;
    }

    //------------------------------------------------------------

    /*
    3-----0
    |     |
    2-----1

    */
    //triangle indices
    vector<tuple<int, int, int>> triangle_groups;
    triangle_groups.pb({0, 1, 2});
    triangle_groups.pb({2, 3, 0});
    this->num_elements_in_indices = 3 * triangle_groups.size();
    this->object = create_player_vao(GL_TRIANGLES, tot_nodes, grid_points, GL_FILL, triangle_groups);
    return;
}

void Player_Obj::set_position(const float x, const float y)
{
    this->position = glm::vec3(x, y, 0);
}

glm::vec2 Player_Obj::draw(glm::mat4 VP, const GLuint program_player_ID)
{
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate(this->position); // glTranslatef
    // glm::mat4 rotate = glm::rotate((float)(this->rotation * M_PI / 180.0f), glm::vec3(1, 1, 1));
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    // Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * translate;
    // std::cout << "MVP   is \n"
    //           << glm::to_string(MVP) << std::endl;
    // GLCall(glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]));
    setMat4("MVP", MVP, program_player_ID);

    glm::vec4 provisional_pos=MVP*glm::vec4(this->position,1.0f);
    GLCall(draw_player(this->object, this->num_elements_in_indices));
    return glm::vec2(provisional_pos.x, provisional_pos.y);

    // void setMat4(const std::string &name, const glm::mat4 &smat, int prog_id)

}

/* Generate VAO, VBOs and return VAO handle */
//this is the 3rd one called
struct VAO *create_player_vao(const GLenum primitive_mode, const int numVertices,
                              const GLfloat *vertex_buffer_data,
                              GLenum fill_mode, const vector<tuple<int, int, int>> &triangle_groups)
{
    struct VAO *vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    GLCall(glGenVertexArrays(1, &(vao->VertexArrayID))); // VAO
    GLCall(glGenBuffers(1, &(vao->VertexBuffer)));       // VBO - vertices
    // GLCall(glGenBuffers(1, &(vao->ColorBuffer)));        // VBO - colorss
    GLCall(glGenBuffers(1, &(vao->IndexBuffer))); //VBO -> INDICES

    GLCall(glBindVertexArray(vao->VertexArrayID));                                                                // Bind the VAO
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer));                                                     // Bind the VBO vertices
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * numVertices * sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW)); // Copy the vertices into VBO

    ///////////////////////////////////////////////////////

    //important to to this only after VAO is bound
    //attribute id, number of dimensions. type of data in the cells, normalize ? , gap between consecutive vertices, offset for this particular attribute
    // position attribute
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0));
    GLCall(glEnableVertexAttribArray(0));
    // texture coord attribute
    GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))));
    GLCall(glEnableVertexAttribArray(1));

    /////////////////////////////////////////////////
    const int ib_sz = triangle_groups.size() * 3;
    unsigned int indices[ib_sz];

    for (int i = 0; i < triangle_groups.size(); i++)
    {
        tie(indices[3 * i], indices[3 * i + 1], indices[3 * i + 2]) = triangle_groups[i];
    }
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->IndexBuffer));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
    //---------------------------------------------------------------------------

    return vao;
}
