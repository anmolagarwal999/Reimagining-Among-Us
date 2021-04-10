#ifndef MAIN_H
#define MAIN_H
#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>    // std::random_shuffle
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <tuple>
#include <map>
#include <chrono> //for time


// #include <glad/glad.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

//anmolagarwal999
#define pb push_back
#define all(c) (c).begin(), (c).end()
#define debug(x) cout << #x << " : " << x << endl
#define part cout << "----------------------------------\n";
typedef long long LL;

struct color_t
{
    int r;
    int g;
    int b;
};

//main.cpp
void processInput(GLFWwindow *window);
void print_vec3(glm::vec3 a);
bool did_crewmate_collide_with_wall(glm::vec3 pos_vec);


// nonedit.cpp
GLFWwindow *initGLFW(int width, int height);
GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path);

struct VAO *create3DObject(GLenum primitive_mode,
                           int numVertices,
                           const GLfloat *vertex_buffer_data,
                           const color_t color,
                           GLenum fill_mode,
                           std::vector<unsigned int> &indices_vec);
struct VAO *create3DObject(GLenum primitive_mode,
                           int numVertices,
                           const GLfloat *vertex_buffer_data,
                           const GLfloat red, const GLfloat green,
                           const GLfloat blue,
                           GLenum fill_mode,
                           std::vector<unsigned int> &indices_vec);

struct VAO *create3DObject(GLenum primitive_mode, int numVertices,
                           const GLfloat *vertex_buffer_data,
                           const GLfloat *color_buffer_data,
                           GLenum fill_mode, std::vector<unsigned int> &indices_vec);

void draw3DObject(struct VAO *vao, unsigned int num_elements_in_indices);
void draw_maze(struct VAO *vao, unsigned int num_elements_in_indices);
void draw_player(struct VAO *vao, unsigned int num_elements_in_indices);
void draw_impostor(struct VAO *vao, unsigned int num_elements_in_indices);
void draw_props(struct VAO *vao, unsigned int num_elements_in_indices);
void draw_background(struct VAO *vao, unsigned int num_elements_in_indices);



// input.cpp
void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods);
void keyboardChar(GLFWwindow *window, unsigned int key);
void mouseButton(GLFWwindow *window, int button, int action, int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// other_handlers.cpp
void error_callback(int error, const char *description);
void quit(GLFWwindow *window);
void reshapeWindow(GLFWwindow *window, int width, int height);
void setMat4(const std::string &name, const glm::mat4 &mat, int prog_id);


// error_checker.cpp
#define ASSERT(x) \
    if (!(x))     \
        __builtin_trap();

bool GLCheckError(const char *function, const char *file, int line);

#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLCheckError(#x, __FILE__, __LINE__));

//************************************************************************************************
void GLClearError();
bool GLCheckError(const char *function, const char *file, int line);

// Types
struct VAO
{
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;
    GLuint IndexBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices
{
    glm::mat4 projection;
    glm::mat4 model;
    glm::mat4 view;
    GLuint MatrixID;
};

extern GLMatrices Matrices;

// ---- Logic ----

enum direction_t
{
    DIR_UP,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT
};

struct bounding_box_t
{
    float x;
    float y;
    float width;
    float height;
};

bool detect_collision(bounding_box_t a, bounding_box_t b);

extern float screen_zoom, screen_center_x, screen_center_y;
void reset_screen(int new_height, int new_width);




// ---- Colors ----
extern const color_t COLOR_RED;
extern const color_t COLOR_GREEN;
extern const color_t COLOR_BLACK;
extern const color_t COLOR_BACKGROUND;



//Useful global variables
extern float start_square_center_x;
extern float start_square_center_y;
extern int ORTHO_LEFT_X;
extern int ORTHO_RIGHT_X;
extern int ORTHO_LEFT_Y;
extern int ORTHO_RIGHT_Y;
extern int ORTHO_LEFT_Z;
extern int ORTHO_RIGHT_Z;
extern int NUM_COLS;
extern int NUM_ROWS ;
extern float COLS_SPAN;
extern float ROWS_SPAN ;

// extern vector<pair<int, int>> adj[100][100];
extern char direction_to_pursue[100][100][100][100];


#endif
