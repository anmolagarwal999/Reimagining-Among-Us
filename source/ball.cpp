#include "ball.h"
#include "main.h"
#include <glm/gtx/string_cast.hpp>

#define GLM_ENABLE_EXPERIMENTAL
using namespace std;

Geometric_Obj::Geometric_Obj(float x, float y, color_t color, int choice)
{
    if (choice == 1)
    {
        this->position = glm::vec3(x, y, 0);
        this->rotation = 0;
        speed = 1;

        //----------------------------------------------------------
        GLfloat vertex_buffer_data[10 * 3];

        int num_features = 3;
        for (int i = 0; i < 10; i++)
        {
            if (i == 0)
            {
                //top center
                //x=0, y=0.5, z=0
                vertex_buffer_data[num_features * i] = 0.0f;
                vertex_buffer_data[num_features * i + 1] = 1.0f;
                vertex_buffer_data[num_features * i + 2] = 0.0f;
                // vertex_buffer_data[5 * i + 3] = 0.0f;
                // vertex_buffer_data[5 * i + 4] = 0.0f;
            }
            else if (i < 5)
            {
                //x,z variable, y=0
                float angle = (360.0f / 4.0f) * (i - 1) * M_PI / 180.0f;
                vertex_buffer_data[num_features * i] = (float)cos(angle);
                vertex_buffer_data[num_features * i + 1] = (float)0.0f;
                vertex_buffer_data[num_features * i + 2] = (float)sin(angle);
                // vertex_buffer_data[5 * i + 3] = 1.0f;
                // if (i % 2 == 0)
                // {
                //     vertex_buffer_data[5 * i + 3] = 0.0f;
                // }
                // vertex_buffer_data[5 * i + 4] = 1.0f;
            }
            else if (i == 5)
            {
                //bottom center
                vertex_buffer_data[num_features * i] = 0.0f;
                vertex_buffer_data[num_features * i + 1] = -2.0f;
                vertex_buffer_data[num_features * i + 2] = 0.0f;
                // vertex_buffer_data[5 * i + 3] = 0.0f;
                // vertex_buffer_data[5 * i + 4] = 0.0f;
            }
            else
            {
                float angle = (360.0f / 4.0f) * (i - 5 - 1) * M_PI / 180.0f;
                vertex_buffer_data[num_features * i] = (float)cos(angle);
                vertex_buffer_data[num_features * i + 1] = (float)(-1.0f);
                vertex_buffer_data[num_features * i + 2] = (float)sin(angle);
                // vertex_buffer_data[5 * i + 3] = 1.0f;
                // if (i % 2 == 0)
                // {
                //     vertex_buffer_data[5 * i + 3] = 0.0f;
                // }
                // vertex_buffer_data[5 * i + 4] = 1.0f;
            }
        }
        for (int i = 0; i < 10; i++)
        {
            cout << i << " -> "
                 << "Vertex is "
                 << vertex_buffer_data[num_features * i]
                 << " : " << vertex_buffer_data[num_features * i + 1]
                 << " : " << vertex_buffer_data[num_features * i + 2]
                 //  << " : " << vertex_buffer_data[num_features * i + 3]
                 //  << " : " << vertex_buffer_data[num_features * i + 4]
                 << " : " << endl;
        }

        //------------------------------------------------------------

        int num_participating_vertices = (sizeof(vertex_buffer_data) / sizeof(GLfloat)) / 3;
        std::cout << "Num of participating vertices is " << num_participating_vertices << std::endl;

        int indices_sz = 16 * 3;
        std::vector<unsigned int> indices_vec(indices_sz, -1);

        unsigned int indices[indices_sz] = {
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 1,

            5, 6, 7,
            5, 7, 8,
            5, 8, 9,
            5, 9, 6,

            1, 2, 6,
            7, 2, 6,

            3, 2, 8,
            7, 2, 8,

            3, 4, 9,
            3, 8, 9,

            4, 1, 9,
            6, 1, 9

        };
        for (int i = 0; i < indices_sz; i++)
        {
            indices_vec[i] = indices[i];
            // cout << "indices vec " << i << " is " << indices_vec[i] << endl;
        }
        this->num_elements_in_indices = indices_vec.size();
        this->object = create3DObject(GL_TRIANGLES, num_participating_vertices, vertex_buffer_data, color, GL_FILL, indices_vec);
        vector<glm::vec3> displacement_offsets;
        displacement_offsets.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
        displacement_offsets.push_back(glm::vec3(1.0f, 10.0f, 1.0f));
        displacement_offsets.push_back(glm::vec3(-5.0f, -10.0f, -5.0f));
        this->displacement_positions = displacement_offsets;
    }
    else if (choice == 2)
    {
        this->position = glm::vec3(x, y, 0);
        this->rotation = 0;
        speed = 1;

        //----------------------------------------------------------
        // GLfloat vertex_buffer_data[10 * 3];

        int num_features = 3;

        //=============================================================
        GLfloat vertex_buffer_data[8 * 3];

        for (int i = 0; i < 8; i++)
        {
            if (i == 0)
            {
                //top center
                //x=0, y=0.5, z=0
                vertex_buffer_data[num_features * i] = 0.0f;
                vertex_buffer_data[num_features * i + 1] = -1.0f;
                vertex_buffer_data[num_features * i + 2] = 0.0f;
            }
            else if (i != 7)
            {
                //x,z variable, y=0
                float angle = 60.0 * (i - 1) * M_PI / 180.0f;
                vertex_buffer_data[num_features * i] = (float)cos(angle);
                vertex_buffer_data[num_features * i + 1] = (float)0.0f;
                vertex_buffer_data[num_features * i + 2] = (float)sin(angle);
            }
            else
            {
                //below center
                //x=0, y=-0.5, z=0
                cout << "i is " << i << endl;

                vertex_buffer_data[num_features * i] = 0.0f;
                vertex_buffer_data[num_features * i + 1] = 1.0f;
                vertex_buffer_data[num_features * i + 2] = 0.0f;
            }
        }

        for (int i = 0; i < 8; i++)
        {
            cout << i << " -> "
                 << "Vertex is "
                 << vertex_buffer_data[num_features * i]
                 << " : " << vertex_buffer_data[num_features * i + 1]
                 << " : " << vertex_buffer_data[num_features * i + 2]
                 //  << " : " << vertex_buffer_data[num_features * i + 3]
                 //  << " : " << vertex_buffer_data[num_features * i + 4]
                 << " : " << endl;
        }
        //------------------------------------------------------------

        int num_participating_vertices = (sizeof(vertex_buffer_data) / sizeof(GLfloat)) / 3;
        std::cout << "Num of participating vertices is " << num_participating_vertices << std::endl;

        int indices_sz = 12 * 3;
        std::vector<unsigned int> indices_vec(indices_sz, -1);

        unsigned int indices[indices_sz];

        int now = 1;
        for (int i = 0; i < 12; i++)
        {
            if (i < 6)
            {
                indices[3 * i] = 0;
            }
            else
            {
                indices[3 * i] = 7;
            }
            indices[3 * i + 1] = now;
            int next = (now + 1) % 7;
            if (next == 0)
            {
                next = 1;
            }
            indices[3 * i + 2] = next;
            now = next;
            cout << "index is " << indices[3 * i] << "--" << indices[3 * i + 1] << "--" << indices[3 * i + 2] << "--" << endl;
        }
        for (int i = 0; i < indices_sz; i++)
        {
            indices_vec[i] = indices[i];
            // cout << "indices vec " << i << " is " << indices_vec[i] << endl;
        }
        this->num_elements_in_indices = indices_vec.size();
        this->object = create3DObject(GL_TRIANGLES, num_participating_vertices, vertex_buffer_data, color, GL_FILL, indices_vec);
        vector<glm::vec3> displacement_offsets;
        displacement_offsets.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
        displacement_offsets.push_back(glm::vec3(1.0f, 10.0f, 1.0f));
        displacement_offsets.push_back(glm::vec3(-5.0f, -10.0f, -5.0f));
        this->displacement_positions = displacement_offsets;
    }

    else if (choice == 3)
    {
        this->position = glm::vec3(x, y, 0);
        this->rotation = 0;
        speed = 1;

        //----------------------------------------------------------
        // GLfloat vertex_buffer_data[10 * 3];

        int num_features = 3;

        //=============================================================
        GLfloat vertex_buffer_data[13 * 3];

        for (int i = 0; i < 13; i++)
        {
            if (i == 0)
            {
                //top center
                //x=0, y=0.5, z=0
                vertex_buffer_data[num_features * i] = 0.0f;
                vertex_buffer_data[num_features * i + 1] = 2.0f;
                vertex_buffer_data[num_features * i + 2] = 0.0f;
                // vertex_buffer_data[5 * i + 3] = 0.0f;
                // vertex_buffer_data[5 * i + 4] = 0.0f;
            }
            else if (i != 12)
            {
                //x,z variable, y=0
                float angle = (360.0f / 11.0f) * (i - 1) * M_PI / 180.0f;
                vertex_buffer_data[num_features * i] = 2 * (float)cos(angle);
                vertex_buffer_data[num_features * i + 1] = (float)0.0f;
                vertex_buffer_data[num_features * i + 2] = 2 * (float)sin(angle);
                // vertex_buffer_data[5 * i + 3] = 1.0f;
                // if (i % 2 == 0)
                // {
                //     vertex_buffer_data[5 * i + 3] = 0.0f;
                // }
                // vertex_buffer_data[5 * i + 4] = 1.0f;
            }
            else
            {
                vertex_buffer_data[num_features * i] = 0.0f;
                vertex_buffer_data[num_features * i + 1] = (float)0.0f;
                vertex_buffer_data[num_features * i + 2] = 0.0f;
            }
        }

        for (int i = 0; i < 13; i++)
        {
            cout << i << " -> "
                 << "Vertex is "
                 << vertex_buffer_data[num_features * i]
                 << " : " << vertex_buffer_data[num_features * i + 1]
                 << " : " << vertex_buffer_data[num_features * i + 2]
                 //  << " : " << vertex_buffer_data[num_features * i + 3]
                 //  << " : " << vertex_buffer_data[num_features * i + 4]
                 << " : " << endl;
        }
        //------------------------------------------------------------

        int num_participating_vertices = (sizeof(vertex_buffer_data) / sizeof(GLfloat)) / 3;
        std::cout << "Num of participating vertices is " << num_participating_vertices << std::endl;

        int indices_sz = 11 * 3 * 2;
        std::vector<unsigned int> indices_vec(indices_sz, -1);

        unsigned int indices[indices_sz];
        int now = 1;
        for (int i = 0; i < 11 * 2; i++)
        {

            if (i < 11)
            {
                indices[3 * i] = 0;
            }
            else
            {
                indices[3 * i] = 12;
            }

            indices[3 * i + 1] = now;
            int next = (now + 1) % 12;
            if (next == 0)
            {
                next = 1;
            }
            indices[3 * i + 2] = next;
            now = next;
            cout << "index is " << indices[3 * i] << "--" << indices[3 * i + 1] << "--" << indices[3 * i + 2] << "--" << endl;
        }
        for (int i = 0; i < indices_sz; i++)
        {
            indices_vec[i] = indices[i];
            // cout << "indices vec " << i << " is " << indices_vec[i] << endl;
        }
        this->num_elements_in_indices = indices_vec.size();
        this->object = create3DObject(GL_TRIANGLES, num_participating_vertices, vertex_buffer_data, color, GL_FILL, indices_vec);
        vector<glm::vec3> displacement_offsets;
        displacement_offsets.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
        displacement_offsets.push_back(glm::vec3(1.0f, 10.0f, 1.0f));
        displacement_offsets.push_back(glm::vec3(-5.0f, -10.0f, -5.0f));
        this->displacement_positions = displacement_offsets;
    }
}

void Geometric_Obj::draw(glm::mat4 VP)
{
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate(this->position); // glTranslatef
    glm::mat4 rotate = glm::rotate((float)(this->rotation * M_PI / 180.0f), glm::vec3(1, 1, 1));
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    // Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    // std::cout << "VP   is \n"<<glm::to_string(MVP) << std::endl;
    GLCall(glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]));
    GLCall(draw3DObject(this->object, this->num_elements_in_indices));
}

void Geometric_Obj::set_position(float x, float y)
{
    this->position = glm::vec3(x, y, 0);
}

void Geometric_Obj::tick()
{
    this->rotation += speed;
    // this->position.x -= speed;
    // this->position.y -= speed;
}
