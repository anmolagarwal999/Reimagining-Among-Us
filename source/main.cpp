#include "main.h"
#include "timer.h"
#include "ball.h"
#include "maze.h"
#include "player.h"
#include "props.h"
#include "impostor.h"
#include "shader.h"
#include "stb_image.h"
#include "Texture.h"
#include "background.h"
#include <glm/gtx/string_cast.hpp>
using namespace std;

#include "filesystem.h"

#include <ft2build.h>
#include FT_FREETYPE_H

//------------------------------------------------------
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);
Shader text_shader;
struct Text_char_struct
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Text_char_struct> Characters;
unsigned int text_VAO, text_VBO;
//-------------------------------------------------------------
//## Some textures being used
Texture texture_obj;
Texture idle_texture;
vector<Texture> player_textures;
//-----
Texture crown_texture;
Texture bomb_texture;
Texture btn_aladdin_texture;
Texture btn_kill_texture;
Texture bg_texture;

Texture curr_player_texture;
Texture curr_impostor_texture;
vector<Texture> left_running, right_running;
//--------------------------------------------------------------

glm::vec2 player_pos;

//-----------------------------------------------------------
GLMatrices Matrices;

//#####---Shaders---------------
GLuint programID;
GLuint program_maze_ID;
GLuint program_bg_ID;
GLuint program_player_ID;
GLuint program_impostor_ID;
GLuint program_props_ID;
GLuint texture_program_ID;

int curr_left_id = 0, curr_right_id = 0;
int curr_left_impostor_id = 0, curr_right_impostor_id = 0;

//impostor shader same as player shader

GLFWwindow *window;

/**************************
* Customizable functions *
**************************/

//GAME PLAY OBJECTS

Geometric_Obj geo_obj;
Maze_obj maze_obj;
Player_Obj player_obj;
Impostor_Obj impostor_obj;
Background_Obj bg_obj;
vector<Props_Obj> props_list;

char direction_to_pursue[100][100][100][100];

int choice_id;
int master_texture_id = -1;

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;
int ORTHO_LEFT_X;
int ORTHO_RIGHT_X;
int ORTHO_LEFT_Y;
int ORTHO_RIGHT_Y;
int ORTHO_LEFT_Z;
int ORTHO_RIGHT_Z;
int NUM_COLS;
int NUM_ROWS;
float COLS_SPAN;
float ROWS_SPAN;

//------------------------------

//------------------------------

const int tot_tasks = 2;
const int number_frames_for_darkness_bonus = 10;
int num_epoches_since_darkness = 0;
int tasks_completed = 0;
long long curr_score = 0;
int global_draw_cnt = 0;
const float TOT_SECS_AVAILABLE = 120.0f;

float start_square_center_x;
float start_square_center_y;

//maze shader details
int maze_shader_model_id;
int maze_shader_view_id;
int maze_shader_proj_id;

//player shader mvp id
int player_shader_mvp_id;

////////////////////////////////////////////
//-------------------GAME STATUS------------------------//
bool btn_kill_pressed = false;
bool btn_aladdin_pressed = false;
bool impostor_alive = true;
bool crewmate_alive = true;
bool game_ongoing = true;
bool player_killed = false;
bool time_gone = false;
bool player_won = false;
bool load_texture = false;
bool lightning_on = false;

//-----------------------------------------------------------------

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;
chrono::high_resolution_clock::time_point start_time, current_time;

//-------------------------------------------------------------

Timer t60(1.0 / 70);

tuple<int, int> get_cell_coordinates_from_real_world_coordinates(const float &center_x, const float &center_y)
{
    int x_before = floor((center_x - ORTHO_LEFT_X) / COLS_SPAN);
    int y_before = floor((center_y - ORTHO_LEFT_Y) / ROWS_SPAN);
    // x_before+=1;
    // y_before+=1;
    return {x_before, y_before};
}
template <typename TYPE_1, typename TYPE_2>
bool check_collision(TYPE_1 obj_1, TYPE_2 obj_2)
{
    float x1_left = obj_1.position.x - obj_1.x_length_rect / 2.0;
    float x1_right = obj_1.position.x + obj_1.x_length_rect / 2.0;
    float y1_left = obj_1.position.y - obj_1.y_length_rect / 2.0;
    float y1_right = obj_1.position.y + obj_1.y_length_rect / 2.0;

    float x2_left = obj_2.position.x - obj_2.x_length_rect / 2.0;
    float x2_right = obj_2.position.x + obj_2.x_length_rect / 2.0;
    float y2_left = obj_2.position.y - obj_2.y_length_rect / 2.0;
    float y2_right = obj_2.position.y + obj_2.y_length_rect / 2.0;

    if (x1_left < x2_right && x1_right > x2_left &&
        y1_left < y2_right && y1_right > y2_left)
    {
        return true;
    }
    return false;
}

template <typename TYPE_1, typename TYPE_2>
float get_distance(TYPE_1 obj_1, TYPE_2 obj_2)
{
    float dx = obj_1.position.x - obj_2.position.x;
    float dy = obj_1.position.y - obj_2.position.y;
    return dx * dx + dy * dy;
}

void generate_obstacles_and_powerups()
{
    const int quan_each = 2;

    //obstacles
    for (int i = 0; i < 3; i++)
    {
        auto x = Props_Obj(ORTHO_LEFT_X,
                           ORTHO_RIGHT_X,
                           ORTHO_LEFT_Y,
                           ORTHO_RIGHT_Y,
                           ORTHO_LEFT_Z,
                           ORTHO_RIGHT_Z, NUM_COLS, NUM_ROWS, 3, "obstacle\0");

        props_list.pb(x);
    }

    //powerups
    for (int i = 0; i < 6; i++)
    {
        auto x = Props_Obj(ORTHO_LEFT_X,
                           ORTHO_RIGHT_X,
                           ORTHO_LEFT_Y,
                           ORTHO_RIGHT_Y,
                           ORTHO_LEFT_Z,
                           ORTHO_RIGHT_Z, NUM_COLS, NUM_ROWS, 4, "powerup\0");
        props_list.pb(x);
    }

    //powerups
}

bool did_reach_destination_with_tasks()
{
    // debug(tasks_completed);
    // debug(tot_tasks);
    if (tasks_completed < tot_tasks)
    {
        return false;
    }

    auto x = get_cell_coordinates_from_real_world_coordinates(player_obj.position.x, player_obj.position.y);
    if (get<0>(x) == NUM_COLS - 2 && get<1>(x) == NUM_ROWS - 2)
    {
        return true;
    }
    return false;
}
float assert_time_limits()
{
    // chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = current_time - start_time;
    // std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    return TOT_SECS_AVAILABLE - elapsed.count();
}

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw()
{
    // clear the color and depth in the frame buffer
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // debug(get_distance(player_obj, impostor_obj));

    // debug(start_time);
    // player_pos = glm::vec2(player_obj.position.x, player_obj.position.y);

    if (lightning_on == false && game_ongoing)
    {
        num_epoches_since_darkness += 1;
        num_epoches_since_darkness %= number_frames_for_darkness_bonus;
        if (num_epoches_since_darkness == 0)
        {
            curr_score += 1;
        }
    }

    global_draw_cnt += 1;
    global_draw_cnt %= 1;
    if (global_draw_cnt == 0)
    {

        if (crewmate_alive && impostor_alive)
        {
            int target_x = 0;
            int target_y = 0;
            tie(target_x, target_y) = get_cell_coordinates_from_real_world_coordinates(player_obj.position.x, player_obj.position.y);
            // debug(target_x);
            // debug(target_y);
            // debug(impostor_obj.curr_col_num);
            // debug(impostor_obj.curr_row_num);
            int src_x = 0;
            int src_y = 0;
            tie(src_x, src_y) = get_cell_coordinates_from_real_world_coordinates(impostor_obj.position.x, impostor_obj.position.y);
            //find direction
            char dir_now = direction_to_pursue[impostor_obj.curr_col_num][impostor_obj.curr_row_num][target_x][target_y];
            // debug(dir_now);
            if (dir_now == 'R')
            {
                curr_left_impostor_id += 1;
                curr_left_impostor_id %= left_running.size();
                curr_impostor_texture = left_running[curr_left_impostor_id];
            }
            if (dir_now == 'L')
            {
                curr_right_impostor_id += 1;
                curr_right_impostor_id %= right_running.size();
                curr_impostor_texture = right_running[curr_right_impostor_id];
            }
            // curr_right_impostor_id += 1;
            // curr_right_impostor_id %= right_running.size();
            // curr_impostor_texture = right_running[curr_right_impostor_id];
            // int curr_x=Impostor_Obj
            if (dir_now != '-')
            {
                impostor_obj.chase_crewmate(dir_now);
            }
        }
    }

    // use the loaded shader program
    // Don't change unless you know what you are doing
    // GLCall(glUseProgram(programID));

    Matrices.projection = glm::ortho((float)ORTHO_LEFT_X, (float)ORTHO_RIGHT_X,
                                     (float)ORTHO_LEFT_Y, (float)ORTHO_RIGHT_Y,
                                     (float)ORTHO_LEFT_Z, (float)ORTHO_RIGHT_Z);
    // if (load_texture)
    // {

    //     GLCall(glActiveTexture(GL_TEXTURE0));
    //     // debug(master_texture_id);s
    //     GLCall(glBindTexture(GL_TEXTURE_2D, master_texture_id));
    //     GLCall(glUniform1i(glGetUniformLocation(programID, "texture1"), 0));
    //     // glBindTexture(GL_TEXTURE_2D, 0);
    // }

    if (game_ongoing)
    {

        /////////////////////////////////////////////////////////////////////////
        //////////////////////////////
        ////BG

        if (true)
        {

            glUseProgram(program_bg_ID);
            glm::mat4 bg_VP = Matrices.projection;
            GLCall(glActiveTexture(GL_TEXTURE1));
            GLCall(glBindTexture(GL_TEXTURE_2D, bg_texture.m_RendererID));
            GLCall(glUniform1i(glGetUniformLocation(program_bg_ID, "texture1"), 1));
            GLCall(glUniform2f(glGetUniformLocation(program_bg_ID, "origin_coordinates"), player_obj.position.x, player_obj.position.y));

            bg_obj.draw(bg_VP, program_bg_ID);
        }

        ////////////////////////////////////

        //RENDER MAZE////////
        glUseProgram(program_maze_ID);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = Matrices.projection;
        if (lightning_on == true)
        {
            GLCall(glUniform3f(glGetUniformLocation(program_maze_ID, "origin_coordinates"), player_obj.position.x, player_obj.position.y, 0.0f));
        }
        else
        {
            GLCall(glUniform3f(glGetUniformLocation(program_maze_ID, "origin_coordinates"), player_obj.position.x, player_obj.position.y, 1.0f));
        }

        setMat4("model", model, program_maze_ID);
        setMat4("view", view, program_maze_ID);
        setMat4("projection", projection, program_maze_ID);
        maze_obj.draw();

        ///////////////////////////////////////////////////

        if (crewmate_alive)
        {
            // curr_left_id += 1;
            // curr_left_id %= right_running.size();
            glUseProgram(program_player_ID);
            glm::mat4 player_VP = Matrices.projection;
            GLCall(glActiveTexture(GL_TEXTURE1));
            // debug(master_texture_id);s
            GLCall(glBindTexture(GL_TEXTURE_2D, curr_player_texture.m_RendererID));
            // GLCall(glBindTexture(GL_TEXTURE_2D, bomb_texture.m_RendererID));
            GLCall(glUniform1i(glGetUniformLocation(program_player_ID, "texture1"), 1));

            player_pos = player_obj.draw(player_VP, program_player_ID);
        }

        //--------------------------------------------------------------------------------------

        //RENDER IMPOSOR////////

        if (impostor_alive)
        {
            glUseProgram(program_impostor_ID);
            glm::mat4 impostor_VP = Matrices.projection;
            // cout << "Drawing impostor" << endl;
            if (lightning_on == false)
            {
                GLCall(glUniform2f(glGetUniformLocation(program_impostor_ID, "dis"), get_distance(player_obj, impostor_obj), 0));
            }
            else
            {
                GLCall(glUniform2f(glGetUniformLocation(program_impostor_ID, "dis"), 0, 0));
            }

            GLCall(glActiveTexture(GL_TEXTURE1));
            // debug(curr_impostor_texture.m_RendererID);
            GLCall(glBindTexture(GL_TEXTURE_2D, curr_impostor_texture.m_RendererID));
            GLCall(glUniform1i(glGetUniformLocation(program_impostor_ID, "texture1"), 1));

            impostor_obj.draw(impostor_VP, program_player_ID);
        }

        ///////////////////////////////////////////////////
        // part;
        // cout << "PROPS STUFF\n";
        //RENDER BUTTONS

        int tracking_idx = 0;
        for (auto &x : props_list)
        {
            glm::mat4 props_VP = Matrices.projection;
            bool proceed_with_draw = true;
            if (tracking_idx == 0)
            {
                proceed_with_draw = !btn_kill_pressed;
            }
            else if (tracking_idx == 1)
            {
                proceed_with_draw = !btn_aladdin_pressed;
            }

            if (proceed_with_draw && x.is_visible)
            {
                // debug(tracking_idx);
                // glUseProgram(program_impostor_ID);

                // GLCall(glActiveTexture(GL_TEXTURE2));
                Texture *chosen_texture = nullptr;
                if (x.desc[0] == 'p')
                {
                    chosen_texture = &crown_texture;
                }
                else if (x.desc[0] == 'o')
                {
                    chosen_texture = &bomb_texture;
                }
                else
                {
                    if (tracking_idx == 0)
                    {
                        chosen_texture = &btn_kill_texture;
                    }
                    else
                    {
                        chosen_texture = &btn_aladdin_texture;
                    }
                }

                //----------------------------------------
                GLCall(glUseProgram(program_props_ID));
                glm::mat4 player_VP = Matrices.projection;
                GLCall(glActiveTexture(GL_TEXTURE5));
                // debug(master_texture_id);s
                // GLCall(glBindTexture(GL_TEXTURE_2D, left_running[curr_left_id].m_RendererID));
                GLCall(glBindTexture(GL_TEXTURE_2D, (*chosen_texture).m_RendererID));

                if (lightning_on == false)
                {
                    GLCall(glUniform2f(glGetUniformLocation(program_props_ID, "dis"), get_distance(player_obj, x), 0));
                }
                else
                {
                    GLCall(glUniform2f(glGetUniformLocation(program_props_ID, "dis"), 0, 0));
                }

                GLCall(glUniform1i(glGetUniformLocation(program_props_ID, "texture1"), 5));

                x.draw(player_VP, program_props_ID);
                //---------------------------------------------
            }
            tracking_idx++;
            // break;
        }

        // cout << "PROPS STUFF ENDS\n";
        // part;

        ////////////////////////////////////////////////////////
        //Check for collisions
        if (impostor_alive && crewmate_alive)
        {
            bool did_impostor_crewmate_collide = check_collision(player_obj, impostor_obj);
            if (did_impostor_crewmate_collide)
            {
                crewmate_alive = false;
                player_killed = true;
                game_ongoing = false;
            }
        }

        if (!btn_kill_pressed && crewmate_alive)
        {
            bool did_crewmate_press_kill = check_collision(player_obj, props_list[0]);
            if (did_crewmate_press_kill)
            {
                btn_kill_pressed = true;
                curr_score += 1000;
                impostor_alive = false;
                tasks_completed += 1;
            }
        }

        if (!btn_aladdin_pressed && crewmate_alive)
        {
            bool did_crewmate_press_aladdin = check_collision(player_obj, props_list[1]);
            if (did_crewmate_press_aladdin)
            {
                btn_aladdin_pressed = true;
                curr_score += 500;
                generate_obstacles_and_powerups();
                tasks_completed += 1;
            }
        }

        if (btn_aladdin_pressed && crewmate_alive)
        {
            int p_len = props_list.size();

            for (int i = 2; i < p_len; i++)
            {
                if (props_list[i].is_visible)
                {

                    bool did_hit = check_collision(player_obj, props_list[i]);
                    if (did_hit)
                    {
                        if (props_list[i].desc[0] == 'p')
                        {
                            curr_score += 100;
                            props_list[i].is_visible = false;
                        }
                        else if (props_list[i].desc[0] == 'o')
                        {
                            curr_score -= 2;
                            // props_list[i].is_visible=false;
                        }
                    }
                }
            }
        }

        if (crewmate_alive)
        {
            //check if reached destination
            bool did_win = did_reach_destination_with_tasks();

            if (did_win)
            {
                player_won = true;
                game_ongoing = false;
            }
        }

        // OpenGL state
        // ------------
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float time_left = assert_time_limits();

        std::stringstream ss;
        ss << "Score is " << curr_score << "   |    "
           << "Tasks:" << tasks_completed << "/" << tot_tasks << "  | time_left is " << (int)time_left << "  [ light_stat: " << lightning_on << " ]";
        if (time_left < 0)
        {
            time_gone = true;
            game_ongoing = false;
        }
        string game_details = ss.str();
        RenderText(text_shader, game_details, 0, SCR_HEIGHT - 40, 0.5f, glm::vec3(0.9, 0.9f, 0.9f));
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }
    else
    {
        // OpenGL state
        // ------------
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        std::stringstream ss, ss2;
        string game_details;
        if (player_killed)
        {
            ss << "KILLED :( !!";
        }
        else if (player_won)
        {
            ss << "You won !!";
        }
        else if (time_gone)
        {
            ss << "Time ran out !!";
        }
        game_details = ss.str();
        RenderText(text_shader, game_details, 120, SCR_HEIGHT - 300, 1.3f, glm::vec3(0.9, 0.0f, 0.0f));
        ss2 << "Score :" << curr_score;
        game_details = ss2.str();
        RenderText(text_shader, game_details, 120, SCR_HEIGHT - 600, 1.3f, glm::vec3(0.9, 0.0f, 0.0f));

        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }
}

void tick_input(GLFWwindow *window)
{
    // int left = glfwGetKey(window, GLFW_KEY_LEFT);
    // int right = glfwGetKey(window, GLFW_KEY_RIGHT);
    // if (left)
    // {
    //     // Do something
    // }
}

void tick_elements()
{
}

void populate_left_sprites()
{

    const int tot_poss = 12;
    string common = "../resources/among_us_specific/individual_sprites/Walk/walkcolor00";

    for (int i = 0; i < tot_poss; i++)
    {
        std::stringstream ss;
        if (i + 1 >= 10)
        {
            ss << common << (i + 1) << ".png";
        }
        else
        {

            ss << common << "0" << (i + 1) << ".png";
        }
        string file_path = ss.str();
        Texture tmp_texture = Texture(file_path, 0);
        // tmp_texture.Bind(1);
        left_running.pb(tmp_texture);
    }
}

void populate_right_sprites()
{

    const int tot_poss = 12;
    string common = "../resources/among_us_specific/individual_sprites/Walk_2/output";

    for (int i = 0; i < tot_poss; i++)
    {
        std::stringstream ss;
        if (true)
        {
            ss << common << (i + 1) << ".png";
        }
        else
        {

            ss << common << "0" << (i + 1) << ".png";
        }
        string file_path = ss.str();
        Texture tmp_texture = Texture(file_path, 0);
        // tmp_texture.Bind(1);
        right_running.pb(tmp_texture);
    }
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height)
{

    ORTHO_LEFT_X = -20;
    ORTHO_RIGHT_X = ORTHO_LEFT_X + 40;
    ORTHO_LEFT_Y = -20;
    ORTHO_RIGHT_Y = ORTHO_LEFT_Y + 40;
    ORTHO_LEFT_Z = -20;
    ORTHO_RIGHT_Z = ORTHO_LEFT_Z + 50;

    NUM_COLS = 16 + 1;
    NUM_ROWS = 16 + 1;
    COLS_SPAN = (ORTHO_RIGHT_X - ORTHO_LEFT_X) / (float)(NUM_COLS - 1);
    ROWS_SPAN = (ORTHO_RIGHT_Y - ORTHO_LEFT_Y) / (float)(NUM_ROWS - 1);
    // debug(COLS_SPAN);
    // debug(COLS_SPAN);
    //    debug(COLS_SPAN);
    //    debug(ROWS_SPAN);

    // debug(ORTHO_LEFT_X);
    // debug(ORTHO_RIGHT_X);
    // debug(ORTHO_LEFT_Y);
    // debug(ORTHO_RIGHT_Y);
    // debug(ORTHO_LEFT_Z);
    // debug(ORTHO_RIGHT_Z);

    ///////////////////////////////////////////////////////////////////////////////////

    /* Objects should be created before any other gl function and shaders */
    // Create the models
    float diff = 0.0f;
    geo_obj = Geometric_Obj(0, 0, COLOR_RED, choice_id);
    maze_obj = Maze_obj(ORTHO_LEFT_X,
                        ORTHO_RIGHT_X,
                        ORTHO_LEFT_Y,
                        ORTHO_RIGHT_Y - diff,
                        ORTHO_LEFT_Z,
                        ORTHO_RIGHT_Z, NUM_COLS, NUM_ROWS);

    // //player init after grid only as we are manipulating global variables which need to be used by the player class
    player_obj = Player_Obj(
        start_square_center_x,
        start_square_center_y);

    bg_obj = Background_Obj(
        width,
        height);
    // bg_obj = Background_Obj(
    //     14.0f,
    //     14.0f);

    impostor_obj = Impostor_Obj(ORTHO_LEFT_X,
                                ORTHO_RIGHT_X,
                                ORTHO_LEFT_Y,
                                ORTHO_RIGHT_Y - diff,
                                ORTHO_LEFT_Z,
                                ORTHO_RIGHT_Z, NUM_COLS, NUM_ROWS);

    //one button to vaporize impostor
    Props_Obj btn_kill = Props_Obj(ORTHO_LEFT_X,
                                   ORTHO_RIGHT_X,
                                   ORTHO_LEFT_Y,
                                   ORTHO_RIGHT_Y - diff,
                                   ORTHO_LEFT_Z,
                                   ORTHO_RIGHT_Z, NUM_COLS, NUM_ROWS, 0, "btn_kill\0");
    //one button to activate objects
    Props_Obj btn_aladdin = Props_Obj(ORTHO_LEFT_X,
                                      ORTHO_RIGHT_X,
                                      ORTHO_LEFT_Y,
                                      ORTHO_RIGHT_Y - diff,
                                      ORTHO_LEFT_Z,
                                      ORTHO_RIGHT_Z, NUM_COLS, NUM_ROWS, 1, "btn_aladdin\0");

    props_list.pb(btn_kill);
    props_list.pb(btn_aladdin);

    ////////////////////////////--------------------------

    //---------------------------------------------------

    // Create and compile our GLSL program from the shaders

    if (false)
    {
    }
    else
    {
        programID = LoadShaders("../source/shaders/shader.vert", "../source/shaders/shader_texture.frag");
        program_maze_ID = LoadShaders("../source/shaders/shader_maze.vert", "../source/shaders/shader_maze.frag");
        program_impostor_ID = LoadShaders("../source/shaders/shader.vert", "../source/shaders/shader_texture_impostor.frag");
        program_player_ID = programID;
        program_bg_ID = LoadShaders("../source/shaders/shader_bg.vert", "../source/shaders/shader_bg.frag");
        program_props_ID = LoadShaders("../source/shaders/shader.vert", "../source/shaders/shader_texture_props.frag");

        // load and create a texture
        // -------------------------
      

        // Texture tmp_texture = Texture("/home/anmolagarwal/Desktop/cg_1/hello_9/res/Textures/barca.png");
        Texture tmp_texture = Texture("../resources/among_us_specific/individual_sprites/Walk/walkcolor0001.png", 0);
        tmp_texture.Bind(1);
        player_textures.pb(tmp_texture);

        //-----------------------------------------------------
        crown_texture = Texture("../resources/among_us_specific/crown.png", 0);
        btn_kill_texture = Texture("../resources/among_us_specific/btn_kill.png", 0);
        bomb_texture = Texture("../resources/among_us_specific/bomb5.png", 0);
        btn_aladdin_texture = Texture("../resources/among_us_specific/btn_aladdin.png", 0);
        idle_texture = Texture("../resources/among_us_specific/individual_sprites/idle.png", 0);
        curr_player_texture = idle_texture;
        curr_impostor_texture = idle_texture;
        bg_texture = Texture("../resources/among_us_specific/bg.png", 1);

        //--------------------------------------------------------

        populate_left_sprites();
        populate_right_sprites();

        cout << "1) prog id is " << programID << endl;
        // GLCall(stbi_image_free(data));
        GLCall(glUseProgram(programID));

        // cout << "1) prog id is " << programID << endl;
        // // void glUniform1i(	GLint location,
        // //  	GLint v0);

        cout << glGetUniformLocation(programID, "texture1") << endl;

        // GLCall(glUniform1i(glGetUniformLocation(programID, "texture1"), 0));
        GLCall(glUniform1i(glGetUniformLocation(programID, "texture1"), 0));
    }
    // Get a handle for our "MVP" uniform
    cout << "2) prog id is " << programID << endl;
    ;
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////////////

    reshapeWindow(window, width, height);

    // Background color of the scene
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    GLCall(glClearDepth(1.0f));

    // GLCall(glEnable(GL_DEPTH_TEST));
    // GLCall(glDepthFunc(GL_LEQUAL));

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int setup_text()
{
    // compile and setup the shader
    // ----------------------------
    // Shader shader("text.vs", "text.fs");
    char vs_path[] = "../source/shaders/text.vs";
    char fs_path[] = "../source/shaders/text.fs";
    text_shader = Shader(vs_path, fs_path);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    // projection = glm::ortho((float)ORTHO_LEFT_X, (float)ORTHO_RIGHT_X,
    //                         (float)ORTHO_LEFT_Y, (float)ORTHO_RIGHT_Y,
    //                         (float)ORTHO_LEFT_Z, (float)ORTHO_RIGHT_Z);
    ;
    // glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

    text_shader.use();
    glUniformMatrix4fv(glGetUniformLocation(text_shader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
    // GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, &MVP[0][0]));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FreeType
    // --------
    FT_Library ft_object;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft_object))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // find path to font
    std::string font_name = FileSystem::getPath("resources/fonts/Antonio-Bold.ttf");
    std::cout << "FONT NAME IS " << font_name << std::endl;

    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft_object, font_name.c_str(), 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else
    {

        /*
The function sets the font's width and height parameters. Setting the width to 0 lets the face dynamically calculate the width based on the given height.
        */
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph
            /*
            A FreeType face hosts a collection of glyphs.
            We can set one of those glyphs as the active glyph by calling FT_Load_Char.
            Here we choose to load the character glyph 'c':
            */
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                //By setting FT_LOAD_RENDER as one of the loading flags, we tell FreeType to create an 8-bit grayscale bitmap image for us that we can access via face->glyph->bitmap
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer);
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Text_char_struct character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)};
            Characters.insert(std::pair<char, Text_char_struct>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft_object);

    glGenVertexArrays(1, &text_VAO);
    glGenBuffers(1, &text_VBO);
    glBindVertexArray(text_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, text_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main(int argc, char **argv)
{
    srand(time(nullptr));
    // int width = 600;
    // int height = 600;

    GLCall(glEnable(GL_DEPTH_TEST));

    choice_id = 1;
    // load_texture=false;
    load_texture = true;

    window = initGLFW(SCR_WIDTH, SCR_HEIGHT);

    /////////////////////////////////////////////////
    //SOme settings for text rendering
    // OpenGL state
    // ------------
    // glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ////////////////////////////////

    initGL(window, SCR_WIDTH, SCR_HEIGHT);
    // camera_finalized = geo_obj.position;

    setup_text();
    start_time = std::chrono::high_resolution_clock::now();

    /* Draw in loop */
    while (!glfwWindowShouldClose(window))
    {
        // Process timers

        if (t60.processTick())
        {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            //=======================================
            curr_player_texture = idle_texture;
            curr_impostor_texture = idle_texture;
            //=======================================

            processInput(window);

            // 60 fps
            // OpenGL Draw commands
            draw();
            // Swap Frame Buffer in double buffering
            GLCall(glfwSwapBuffers(window));

            tick_input(window);
        }

        // Poll for Keyboard and mouse events
        GLCall(glfwPollEvents());
    }

    quit(window);
}

void reset_screen(int new_height, int new_width)
{
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float movement_speed = 7.1 * deltaTime;

    glm::vec3 i_cap = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 j_cap = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 k_cap = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 suspected_pos = player_obj.position;
    bool try_move = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        suspected_pos = player_obj.position + j_cap * movement_speed;
        try_move = true;
        curr_left_id += 1;
        curr_left_id %= left_running.size();
        curr_player_texture = left_running[curr_left_id];
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        suspected_pos = player_obj.position - j_cap * movement_speed;
        try_move = true;
        curr_right_id += 1;
        curr_right_id %= right_running.size();
        curr_player_texture = right_running[curr_right_id];
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        suspected_pos = player_obj.position - i_cap * movement_speed;
        try_move = true;
        curr_right_id += 1;
        curr_right_id %= right_running.size();
        curr_player_texture = right_running[curr_right_id];
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        suspected_pos = player_obj.position + i_cap * movement_speed;
        try_move = true;
        curr_left_id += 1;
        curr_left_id %= left_running.size();
        curr_player_texture = left_running[curr_left_id];
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        lightning_on = !lightning_on;
    }
    if (try_move && did_crewmate_collide_with_wall(suspected_pos) == false)
    {
        player_obj.position = suspected_pos;
        // part;
    }
}

bool did_crewmate_collide_with_wall(glm::vec3 pos_vec)
{

    float curr_mid_x = pos_vec.x;
    float curr_mid_y = pos_vec.y;
    const float left_x = curr_mid_x - player_obj.x_length_rect / 2;
    const float right_x = curr_mid_x + player_obj.x_length_rect / 2;
    const float bottom_y = curr_mid_y - player_obj.y_length_rect / 2;
    const float top_y = curr_mid_y + player_obj.y_length_rect / 2;
    // part;
    // debug(left_x);
    // debug(right_x);
    // debug(top_y);
    // debug(bottom_y);

    //   std::vector<std::pair<float, float>> grid_tracking;
    //     std::vector<std::pair<int, int>> index_pairs;
    for (auto &ptr : maze_obj.index_pairs)
    {
        auto ptr1 = maze_obj.grid_tracking[ptr.first];
        auto ptr2 = maze_obj.grid_tracking[ptr.second];

        if (ptr1.first == ptr2.first)
        {
            // cout << "FOund a vertical line\n";
            // cout << "coordinates are " << ptr1.first << ":" << ptr1.second << "::::::::" << ptr2.first << ":" << ptr2.second << endl;

            //vertical line
            if (ptr1.first <= left_x || ptr1.first >= right_x)
            {
                continue;
            }
            // cout << "in range also" << endl;

            if ((ptr1.second > top_y && ptr2.second > top_y) || (ptr1.second < bottom_y && ptr2.second < bottom_y))
            {
                continue;
            }
            return true;
        }
        else if (ptr1.second == ptr2.second)
        {
            // cout << "FOund a HORIZONTAL line\n";
            // cout << "coordinates are " << ptr1.first << ":" << ptr1.second << "::::::::" << ptr2.first << ":" << ptr2.second << endl;

            //horizontal line
            if (ptr1.second <= bottom_y || ptr1.second >= top_y)
            {
                continue;
            }
            // cout << "in range also" << endl;

            if ((ptr1.first > right_x && ptr2.first > right_x) || (ptr1.first < left_x && ptr2.first < left_x))
            {
                continue;
            }
            return true;
        }
    }
    return false;
}

// render line of text
// -------------------
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(text_VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Text_char_struct ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f}};
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, text_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void print_vec3(glm::vec3 a)
{
    std::cout << a[0] << " : " << a[1] << " : " << a[2] << std::endl;
}

//----------------------------------------------------------------------------------------
