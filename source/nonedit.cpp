#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "main.h"

using namespace std;

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow *initGLFW(int width, int height)
{
    GLFWwindow *window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        // exit(EXIT_FAILURE);
    }

    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3));
    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3));
    GLCall(glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE));
    GLCall(glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE));

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        // exit(EXIT_FAILURE);
    }

    GLCall(glfwMakeContextCurrent(window));
    // Initialize GLEW, Needed in Core profile
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        cout << "Error: Failed to initialise GLEW : " << glewGetErrorString(err) << endl;
        exit(1);
    }
    GLCall(glfwSwapInterval(1));

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
       is different from WindowSize */
    GLCall(glfwSetFramebufferSizeCallback(window, reshapeWindow));
    GLCall(glfwSetWindowSizeCallback(window, reshapeWindow));

    /* Register function to handle window close */
    GLCall(glfwSetWindowCloseCallback(window, quit));

    /* Register function to handle keyboard input */
    GLCall(glfwSetKeyCallback(window, keyboard));      // general keyboard input
    GLCall(glfwSetCharCallback(window, keyboardChar)); // simpler specific character handling

    /* Register function to handle mouse click */
    GLCall(glfwSetMouseButtonCallback(window, mouseButton)); // mouse button clicks
    GLCall(glfwSetScrollCallback(window, scroll_callback));

    return window;
}

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path)
{

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const *VertexSourcePointer = VertexShaderCode.c_str();
    GLCall(glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL));
    GLCall(glCompileShader(VertexShaderID));

    // Check Vertex Shader
    GLCall(glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result));
    GLCall(glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength));
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    GLCall(glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]));
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const *FragmentSourcePointer = FragmentShaderCode.c_str();
    GLCall(glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL));
    GLCall(glCompileShader(FragmentShaderID));

    // Check Fragment Shader
    GLCall(glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result));
    GLCall(glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength));
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    GLCall(glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]));
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    GLCall(glAttachShader(ProgramID, VertexShaderID));
    GLCall(glAttachShader(ProgramID, FragmentShaderID));
    GLCall(glLinkProgram(ProgramID));

    // Check the program
    GLCall(glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result));
    GLCall(glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength));
    std::vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
    GLCall(glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]));
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    GLCall(glDeleteShader(VertexShaderID));
    GLCall(glDeleteShader(FragmentShaderID));

    return ProgramID;
}

/* Generate VAO, VBOs and return VAO handle */
//this is the 3rd one called
struct VAO *create3DObject(GLenum primitive_mode, int numVertices,
                           const GLfloat *vertex_buffer_data,
                           const GLfloat *color_buffer_data,
                           GLenum fill_mode, vector<unsigned int> &indices_vec)
{
    struct VAO *vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    GLCall(glGenVertexArrays(1, &(vao->VertexArrayID))); // VAO
    GLCall(glGenBuffers(1, &(vao->VertexBuffer)));       // VBO - vertices
    GLCall(glGenBuffers(1, &(vao->ColorBuffer)));        // VBO - colors
    GLCall(glGenBuffers(1, &(vao->IndexBuffer)));        //VBO -> INDICES

    GLCall(glBindVertexArray(vao->VertexArrayID));                                                                // Bind the VAO
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer));                                                     // Bind the VBO vertices
    GLCall(glBufferData(GL_ARRAY_BUFFER, 3 * numVertices * sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW)); // Copy the vertices into VBO

    GLCall(glVertexAttribPointer(
        0,        // attribute 0. Vertices
        3,        // size (x,y,z)
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
        ));
    GLCall(glEnableVertexAttribArray(0));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer));                                                     // Bind the VBO colors
    GLCall(glBufferData(GL_ARRAY_BUFFER, 3 * numVertices * sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW)); // Copy the vertex colors
    GLCall(glVertexAttribPointer(
        1,        // attribute 1. Color
        3,        // size (r,g,b)
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
        ));
    GLCall(glEnableVertexAttribArray(1));

    //-------------------------------------------------------------------------------
    int sz_indices = indices_vec.size();
    unsigned int indices[sz_indices];
    for (int i = 0; i < sz_indices; i++)
    {
        indices[i] = indices_vec[i];
    }
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->IndexBuffer));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
    //---------------------------------------------------------------------------

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */

int hex_to_col(char a, char b)
{
    int val = 0;
    int d;

    if (a > '9')
    {
        //a is letter
        d = a - 'a' + 10;
      //  cout << "d is " << d << endl;
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
      //  cout << "d is " << d << endl;
    }
    else
    {
        d = b - '0';
    }
    val += d;
   // cout << "ret for " << val << endl;

    return val;
}
//this is the 2nd one called
struct VAO *create3DObject(GLenum primitive_mode,
                           int numVertices,
                           const GLfloat *vertex_buffer_data,
                           const GLfloat red, const GLfloat green,
                           const GLfloat blue,
                           GLenum fill_mode,
                           vector<unsigned int> &indices_vec)
{

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
    GLfloat *color_buffer_data = new GLfloat[3 * numVertices];
    for (int i = 0; i < numVertices; i++)
    {
        int ver_idx = (i % colors_arr.size());
        string sel = colors_arr[ver_idx];
        color_buffer_data[3 * i] = hex_to_col(sel[1], sel[2]) / 256.0f;
        color_buffer_data[3 * i + 1] = hex_to_col(sel[3], sel[4]) / 256.0f;
        color_buffer_data[3 * i + 2] = hex_to_col(sel[5], sel[6]) / 256.0f;
        // color_buffer_data[3 * i] = red;
        // color_buffer_data[3 * i + 1] = blue;
        // color_buffer_data[3 * i + 2] = green;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode, indices_vec);
}

//this is the first one called
struct VAO *create3DObject(GLenum primitive_mode,
                           int numVertices,
                           const GLfloat *vertex_buffer_data,
                           const color_t color,
                           GLenum fill_mode,
                           vector<unsigned int> &indices_vec)
{
    return create3DObject(primitive_mode, numVertices, vertex_buffer_data,
                          color.r / 256.0,
                          color.g / 256.0,
                          color.b / 256.0, fill_mode, indices_vec);
}

/* Render the VBOs handled by VAO */
void draw3DObject(struct VAO *vao, unsigned int num_elements_in_indices)
{
    // Change the Fill Mode for this object
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, vao->FillMode));

    // Bind the VAO to use
    GLCall(glBindVertexArray(vao->VertexArrayID));

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->IndexBuffer));

    // Enable Vertex Attribute 0 - 3d Vertices
    // GLCall(glEnableVertexAttribArray(0));
    // // Bind the VBO to use
    // GLCall(glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer));

    // // Enable Vertex Attribute 1 - Color
    // GLCall(glEnableVertexAttribArray(1));
    // // Bind the VBO to use
    // GLCall(glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer));

    // Draw the geometry !
    // GLCall(glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices)); // Starting from vertex 0; 3 vertices total -> 1 triangle

    GLCall(glDrawElements(GL_TRIANGLES, num_elements_in_indices, GL_UNSIGNED_INT, 0));
}

/* Render the VBOs handled by VAO */
void draw_maze(struct VAO *vao, unsigned int num_elements_in_indices)
{
    // Change the Fill Mode for this object
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, vao->FillMode));

    // Bind the VAO to use
    GLCall(glBindVertexArray(vao->VertexArrayID));

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->IndexBuffer));

    GLCall(glDrawElements(GL_LINES, num_elements_in_indices, GL_UNSIGNED_INT, 0));
}

/* Render the VBOs handled by VAO */
void draw_player(struct VAO *vao, unsigned int num_elements_in_indices)
{
    // Change the Fill Mode for this object
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, vao->FillMode));

    // Bind the VAO to use
    GLCall(glBindVertexArray(vao->VertexArrayID));

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->IndexBuffer));

    GLCall(glDrawElements(GL_TRIANGLES, num_elements_in_indices, GL_UNSIGNED_INT, 0));
}


/* Render the VBOs handled by VAO */
void draw_impostor(struct VAO *vao, unsigned int num_elements_in_indices)
{
    // Change the Fill Mode for this object
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, vao->FillMode));

    // Bind the VAO to use
    GLCall(glBindVertexArray(vao->VertexArrayID));

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->IndexBuffer));

    GLCall(glDrawElements(GL_TRIANGLES, num_elements_in_indices, GL_UNSIGNED_INT, 0));
}


/* Render the VBOs handled by VAO */
void draw_props(struct VAO *vao, unsigned int num_elements_in_indices)
{
    // Change the Fill Mode for this object
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, vao->FillMode));

    // Bind the VAO to use
    GLCall(glBindVertexArray(vao->VertexArrayID));

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->IndexBuffer));

    GLCall(glDrawElements(GL_TRIANGLES, num_elements_in_indices, GL_UNSIGNED_INT, 0));
}

/* Render the VBOs handled by VAO */
void draw_background(struct VAO *vao, unsigned int num_elements_in_indices)
{
    // Change the Fill Mode for this object
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, vao->FillMode));

    // Bind the VAO to use
    GLCall(glBindVertexArray(vao->VertexArrayID));

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->IndexBuffer));

    GLCall(glDrawElements(GL_TRIANGLES, num_elements_in_indices, GL_UNSIGNED_INT, 0));
}










