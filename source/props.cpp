#include "props.h"
#include "main.h"
#include <glm/gtx/string_cast.hpp>

#define GLM_ENABLE_EXPERIMENTAL
using namespace std;

int hex_to_col_props(char a, char b)
{
    int val = 0;
    int d;

    if (a > '9')
    {
        //a is letter
        d = a - 'a' + 10;
        // cout << "d is " << d << endl;
    }
    else
    {
        d = a - '0';
    }
    val += d * 16;

    if (b > '9')
    {
        //b is letter
        d = b - 'a' + 10;
        cout << "d is " << d << endl;
    }
    else
    {
        d = b - '0';
    }
    val += d;
    cout << "ret for " << val << endl;

    return val;
}

vector<string> colors_arr{

    // https://mokole.com/palette.html
    "#7f0000",
    "#008000",
    "#000080",
    "#ff8c00",
    "#ffff00",
    "#00ff00",
    "#00ffff",
    "#ff00ff",
    "#1e90ff",
    "#ff69b4",
    "#ffe4c4"

};

struct VAO *create_props_vao(const GLenum primitive_mode, const int numVertices,
                             const GLfloat *vertex_buffer_data,
                             GLenum fill_mode, const vector<tuple<int, int, int>> &triangle_groups);

Props_Obj::Props_Obj(const int &ORTHO_LEFT_X,
                     const int &ORTHO_RIGHT_X,
                     const int &ORTHO_LEFT_Y,
                     const int &ORTHO_RIGHT_Y,
                     const int &ORTHO_LEFT_Z,
                     const int &ORTHO_RIGHT_Z,
                     const int &NUM_COLS,
                     const int &NUM_ROWS,
                     const int &color_idx,
                     const std::string &obj_desc)
{

    // cout << "CONSTRUCTOR FOR PROPS" << endl;

    assert(color_idx < colors_arr.size());
    this->desc = obj_desc;
    this->is_visible = true;
    const float COLS_SPAN = (ORTHO_RIGHT_X - ORTHO_LEFT_X) / ((float)(NUM_COLS - 1));
    const float ROWS_SPAN = (ORTHO_RIGHT_Y - ORTHO_LEFT_Y) / ((float)(NUM_ROWS - 1));
    const int col_num = rand() % (NUM_COLS - 1);
    const int row_num = rand() % (NUM_ROWS - 1);
    this->curr_col_num = col_num;
    this->curr_row_num = row_num;

    // debug(col_num);
    // debug(row_num);

    // debug(COLS_SPAN);
    // debug(ROWS_SPAN);
    //----------------------------------------------
    int i, j, k, t, tmp;
    const float start_square_center_x = ORTHO_LEFT_X + col_num * COLS_SPAN + COLS_SPAN / 2;
    const float start_square_center_y = ORTHO_LEFT_Y + row_num * ROWS_SPAN + ROWS_SPAN / 2;
    //in vertex array, make 2 rectangles in top of each other
    // debug(start_square_center_x);
    // debug(start_square_center_y);
    float middle_x_c = 0.0f, middle_y_c = 0.0f, middle_z_c = 0.0f;
    this->position = glm::vec3(start_square_center_x, start_square_center_y, middle_z_c);

    this->x_length_rect = 2.5f;
    this->y_length_rect = 2.0f;
    // cout << "Drawing impostor" << endl;
    print_vec3(this->position);
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

    float cyan_r = (254.0 / 255.0), cyan_g = (99.0 / 255.0), cyan_b = (71.0 / 255.0);

    string sel_color = colors_arr[color_idx];
    float x_c[] = {1.0f, 1.0f, 0.0f, 0.0f};
    float y_c[] = {1.0f, 0.0f, 0.0f, 1.0f};

    float grid_points[tot_nodes * num_features_per_coordinate];
    for (i = 0; i < tot_nodes; i++)
    {
        grid_points[num_features_per_coordinate * i + 0] = rect_coordinates[i].first;
        grid_points[num_features_per_coordinate * i + 1] = rect_coordinates[i].second;
        grid_points[num_features_per_coordinate * i + 2] = middle_z_c;
        // grid_points[num_features_per_coordinate * i + 3] = hex_to_col_props(sel_color[1], sel_color[2]) / 256.0f;
        // grid_points[num_features_per_coordinate * i + 4] = hex_to_col_props(sel_color[3], sel_color[4]) / 256.0f;
        // grid_points[num_features_per_coordinate * i + 5] = hex_to_col_props(sel_color[5], sel_color[6]) / 256.0f;

        int idx = (i + 0) % 4;
        grid_points[num_features_per_coordinate * i + 3] = x_c[idx];
        grid_points[num_features_per_coordinate * i + 4] = y_c[idx];
        grid_points[num_features_per_coordinate * i + 5] = 0.0f;
    }

    // for (int i = 0; i < tot_nodes; i++)
    // {
    //     cout << i << " -> "
    //          << "Vertex is " << grid_points[num_features_per_coordinate * i] << " : " << grid_points[num_features_per_coordinate * i + 1] << " : " << grid_points[num_features_per_coordinate * i + 2] << " : " << grid_points[num_features_per_coordinate * i + 3] << " : " << grid_points[num_features_per_coordinate * i + 4] << " : " << grid_points[num_features_per_coordinate * i + 5] << " : " << endl;
    // }

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
    this->object = create_props_vao(GL_TRIANGLES, tot_nodes, grid_points, GL_FILL, triangle_groups);
    return;
}

pair<float, float> get_coordinates_from_cell_PROPS(const int &col_num, const int &row_num)
{
    // debug(col_num);
    // debug(row_num);
    const float start_square_center_x = ORTHO_LEFT_X + col_num * COLS_SPAN + COLS_SPAN / 2;
    const float start_square_center_y = ORTHO_LEFT_Y + row_num * ROWS_SPAN + ROWS_SPAN / 2;
    return {start_square_center_x, start_square_center_y};
}

void Props_Obj::draw(glm::mat4 VP, const GLuint program_impostor_ID)
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
    GLCall(setMat4("MVP", MVP, program_impostor_ID));
    // cout << "Drawing impostor" << endl;
    // print_vec3(this->position);
    // void setMat4(const std::string &name, const glm::mat4 &smat, int prog_id)

    GLCall(draw_props(this->object, this->num_elements_in_indices));
}

/* Generate VAO, VBOs and return VAO handle */
//this is the 3rd one called
struct VAO *create_props_vao(const GLenum primitive_mode, const int numVertices,
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
