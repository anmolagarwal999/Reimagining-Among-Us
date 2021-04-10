#include "maze.h"
#include "main.h"
#define GLM_ENABLE_EXPERIMENTAL
using namespace std;
// debug(x_c);
// debug(y_c);
int dx[] = {1, 1, 0, -1, -1, -1, 0, 1}; // trick to explore an implicit 2D grid
int dy[] = {0, 1, 1, 1, 0, -1, -1, -1}; // S,SE,E,NE,N,NW,W,SW neighbors  //GO FOR EVEN FOR 4 moves

bool visited[1000][1000];
bool allow_entry[1000][1000][4];

// vector<pair<int, int>> adj[100][100];

bool dfs(int x_c, int y_c, vector<pair<pair<int, int>, char>> &removals, const int &tot_x_cells, const int &tot_y_cells)
{

    if (!visited[x_c][y_c])
    {
        visited[x_c][y_c] = true;

        //traverse through adjacency matrix
        int i = 4;
        vector<int> idxs({0, 2, 4, 6});
        random_shuffle(all(idxs));

        for (i = 0; i < 4; i += 1)
        {

            int c_x = x_c + dx[idxs[i]];
            int c_y = y_c + dy[idxs[i]];
            if (c_x < 0 || c_x >= tot_x_cells)
            {
                continue;
            }
            if (c_y < 0 || c_y >= tot_y_cells)
            {
                continue;
            }

            bool stat = dfs(c_x, c_y, removals, tot_x_cells, tot_y_cells);

            if (stat == true)
            {
                //traversal possible => push this to adjacency list
                // adj[x_c][y_c].pb({c_x, c_y});

                allow_entry[x_c][y_c][idxs[i] / 2] = true;
                cout << "cells are [" << x_c << ":" << y_c << "] AND [" << c_x << ":" << c_y << "] with " << idxs[i] / 2 << endl;
                cout << "cells are [" << c_x << ":" << c_y << "] AND [" << x_c << ":" << y_c << "] with " << (idxs[i] / 2 + 2) % 4 << endl;
                allow_entry[c_x][c_y][(idxs[i] / 2 + 2) % 4] = true;

                if (idxs[i] == 0)
                {
                    removals.pb({make_pair(x_c + 1, y_c), 'U'});
                }
                else if (idxs[i] == 2)
                {
                    removals.pb({make_pair(x_c, y_c + 1), 'R'});
                }
                else if (idxs[i] == 4)
                {
                    removals.pb({make_pair(x_c, y_c), 'U'});
                }
                else if (idxs[i] == 6)
                {
                    removals.pb({make_pair(x_c, y_c), 'R'});
                }
            }    
       
       
       
        }
        return true;
    }

    return false;
}

void precompute_shortest_paths(const int &num_x_cells, const int &num_y_cells)
{
    int i, j, k, t, temp;
    // cout << "inside precompute shortest paths" << endl;
    // debug(num_x_cells);
    // debug(num_y_cells);

    //init all to fail
    for (i = 0; i < num_x_cells; i++)
    {
        for (j = 0; j < num_y_cells; j++)
        {
            for (k = 0; k < num_x_cells; k++)
            {
                for (t = 0; t < num_y_cells; t++)
                {
                    direction_to_pursue[i][j][k][t] = '-';
                }
            }

            //allow entry statuses
            // cout << "allow entry status for " << i << "-" << j << endl;
            // for (k = 0; k < 4; k++)
            // {
            //     cout << allow_entry[i][j][k] << ":";
            // }
            // cout << endl;
        }
    }

    const LL INF = 1LL << 40;
    LL dis[num_x_cells][num_y_cells][4];

    for (int src_x = 0; src_x < num_x_cells; src_x++)
    {
        for (int src_y = 0; src_y < num_y_cells; src_y++)
        {
            // cout << "###############################################################" << endl;
            // debug(src_x);
            // debug(src_y);
            //init all distances to infinity
            for (k = 0; k < num_x_cells; k++)
            {
                for (t = 0; t < num_y_cells; t++)
                {
                    for (int num = 0; num < 4; num++)
                    {
                        dis[k][t][num] = INF;
                    }
                }
            }
            for (int num = 0; num < 4; num++)
            {
                // debug(num);
                int support_x = src_x + dx[2 * num];
                int support_y = src_y + dy[2 * num];
                // debug(num_x_cells);
                // debug(num_y_cells);
                // debug(support_x);
                // debug(support_y);
                if (support_x < 0 || support_y < 0 || support_x >= num_x_cells || support_y >= num_y_cells)
                {
                    cout << "skipping\n";
                    continue;
                }

                if (allow_entry[src_x][src_y][num] == false)
                {
                    cout << "skipping entry due to wall in between" << endl;
                    continue;
                }
                dis[src_x][src_y][num] = 1;
                dis[support_x][support_y][num] = 0;
                // dis[k][t][num] = INF;

                queue<pair<int, int>> q;
                q.push({support_x, support_y});
                q.push({src_x, src_y});

                while (!q.empty())
                {
                    auto ptr = q.front();
                    q.pop();
                    int x_c = ptr.first;
                    int y_c = ptr.second;

                    for (int i = 0; i < 8; i += 2)
                    {
                        int c_x = x_c + dx[i];
                        int c_y = y_c + dy[i];

                        if (c_x < 0 || c_y < 0 || c_x >= num_x_cells || c_y >= num_y_cells)
                        {
                            continue;
                        }

                        if (allow_entry[x_c][y_c][i / 2] == false)
                        {
                            continue;
                        }

                        if (dis[c_x][c_y][num] == INF)
                        {
                            dis[c_x][c_y][num] = dis[x_c][y_c][num] + 1;
                            q.push({c_x, c_y});
                        }
                    }
                }
            }

            const char dirs[4] = {'R', 'U', 'L', 'D'};
            // for (int num = 0; num < 4; num++)
            // {
            //     part;
            //     cout << "CHAR IS " << dirs[num] << endl;
            //     for (k = 0; k < num_y_cells; k++)
            //     {
            //         for (t = 0; t < num_x_cells; t++)
            //         {
            //             cout << dis[t][k][num] << "=";
            //         }
            //         cout << endl;
            //     }
            // }

            for (k = 0; k < num_y_cells; k++)
            {
                for (t = 0; t < num_x_cells; t++)
                {
                    LL minima = INF + 100;
                    char now = '-';
                    for (int num = 0; num < 4; num++)
                    {
                        if (dis[t][k][num] < minima)
                        {
                            minima = dis[t][k][num];
                            now = dirs[num];
                        }
                    }
                    if (t == src_x && k == src_y)
                    {
                        now = '.';
                    }
                    direction_to_pursue[src_x][src_y][t][k] = now;
                    cout << now << "-";
                }
                cout << endl;
            }
        }

        if (src_x == num_x_cells - 1)
        {
            break;
        }
    }

    return;
}
vector<pair<pair<int, int>, char>> generate_maze(const int num_y_lines, const int num_x_lines)
{
    const int num_y_cells = num_y_lines - 1;
    const int num_x_cells = num_x_lines - 1;
    // assert(num_x_cells <= 5);
    // assert(num_y_cells <= 5);

    //nodes are cells, if go right, delete edge, left, delete edge and so on

    //POINTS VS NODES

    int i, j, k, t, temp;

    for (i = 0; i < num_x_cells; i++)
    {
        for (j = 0; j < num_y_cells; j++)
        {
            visited[i][j] = false;
            for (int k = 0; k < 4; k++)
            {
                allow_entry[i][j][k] = false;
            }
        }
    }

    vector<pair<pair<int, int>, char>> removals;
    // vector<pair<int, int>> removals;

    dfs(0, 0, removals, num_x_cells, num_y_cells);

    precompute_shortest_paths(num_x_cells, num_y_cells);
    return removals;

    //initially all are unvisited
}

/* Generate VAO, VBOs and return VAO handle */
//this is the 3rd one called
struct VAO *create_maze_vao(GLenum primitive_mode, int numVertices,
                            const GLfloat *vertex_buffer_data,
                            GLenum fill_mode, const vector<pair<int, int>> &index_pairs)
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
    GLCall(glBufferData(GL_ARRAY_BUFFER, 5 * numVertices * sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW)); // Copy the vertices into VBO

    ///////////////////////////////////////////////////////

    //important to to this only after VAO is bound
    //attribute id, number of dimensions. type of data in the cells, normalize ? , gap between consecutive vertices, offset for this particular attribute
    // position attribute
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0));
    GLCall(glEnableVertexAttribArray(0));
    // texture coord attribute
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float))));
    GLCall(glEnableVertexAttribArray(1));

    unsigned int indices[index_pairs.size() * 2];

    for (int i = 0; i < index_pairs.size(); i++)
    {
        // debug(i);
        indices[2 * i] = index_pairs[i].first;
        indices[2 * i + 1] = index_pairs[i].second;
        // debug(indices[2 * i]);
        // debug(indices[2 * i + 1]);
        // part;
    }
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->IndexBuffer));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
    return vao;
}

//what we need is number of rows, number of columns,orhto width, ortho height
Maze_obj::Maze_obj(const int &ORTHO_LEFT_X,
                   const int &ORTHO_RIGHT_X,
                   const int &ORTHO_LEFT_Y,
                   const int &ORTHO_RIGHT_Y,
                   const int &ORTHO_LEFT_Z,
                   const int &ORTHO_RIGHT_Z, const int &NUM_COLS, const int &NUM_ROWS)
{

    //find width and length of each cell
    const float COLS_SPAN = (ORTHO_RIGHT_X - ORTHO_LEFT_X) / ((float)(NUM_COLS - 1));
    const float ROWS_SPAN = (ORTHO_RIGHT_Y - ORTHO_LEFT_Y) / ((float)(NUM_ROWS - 1));
    // debug(COLS_SPAN);
    // debug(COLS_SPAN);
    // part;
    // debug(COLS_SPAN);
    // debug(ROWS_SPAN);
    // part;

    //manipulating global varibale for sake of initial position of crewmate
    start_square_center_x = ORTHO_LEFT_X + COLS_SPAN / 2.0;
    start_square_center_y = ORTHO_LEFT_Y + ROWS_SPAN / 2.0;

    //stores the tiny dots (NOT THE CELLS)
    float grid_points[NUM_ROWS * NUM_COLS * 5];
    float row_curr_val = ORTHO_LEFT_Y, col_curr_val = ORTHO_LEFT_X;

    vector<pair<float, float>> grid_tracking;

    int cnt = 0;
    for (int i = 0; i < NUM_COLS; i++)
    {

        row_curr_val = ORTHO_LEFT_Y;
        for (int j = 0; j < NUM_ROWS; j++)
        {
            // debug(row_curr_val);
            //no z-needed, color is white for all
            grid_points[5 * cnt] = col_curr_val;
            grid_points[5 * cnt + 1] = row_curr_val;
            grid_points[5 * cnt + 2] = 0.0f;
            grid_points[5 * cnt + 3] = 0.0f;
            grid_points[5 * cnt + 4] = 0.0f;

            grid_tracking.pb({grid_points[5 * cnt], grid_points[5 * cnt + 1]});

            row_curr_val += ROWS_SPAN;
            cnt++;
        }
        col_curr_val += COLS_SPAN;
    }
    //#########################################################################

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    for (int i = 0; i < NUM_ROWS * NUM_COLS; i++)
    {
        cout << i << " -> "
             << "Vertex is " << grid_points[5 * i] << " : " << grid_points[5 * i + 1] << " : " << grid_points[5 * i + 2] << " : " << grid_points[5 * i + 3] << " : " << grid_points[5 * i + 4] << " : " << endl;
    }
    // debug(NUM_COLS);
    // debug(NUM_ROWS);
    //gets the places where no wall is to be made
    vector<pair<pair<int, int>, char>> removals = generate_maze(NUM_COLS, NUM_ROWS);
    // removals.clear();
    // removals.pb({{3, 1}, 'U'});
    // removals.pb({{2, 3}, 'R'});
    // part;

    // for (auto &x : removals)
    // {
    //     cout << "Removal is " << x.first.first << ":" << x.first.second << "  -:-  " << x.second << endl;
    // }
    // part;
    vector<pair<int, int>> index_pairs;
    cnt = 0;
    for (int i = 0; i < NUM_COLS; i++)
    {
        for (int j = 0; j < NUM_ROWS; j++)
        {
            if (j != NUM_ROWS - 1)
            {

                //right thing
                bool eligible = true;
                for (auto &x : removals)
                {
                    if (x.first.first == i && x.first.second == j && x.second == 'U')
                    {
                        eligible = false;
                    }
                }
                if (eligible)
                {
                    index_pairs.pb({cnt, cnt + 1});
                }
            }
            cnt++;
        }
    }
    cnt = 0;

    for (int i = 0; i < NUM_COLS; i++)
    {
        for (int j = 0; j < NUM_ROWS; j++)
        {
            if (i != NUM_COLS - 1)
            {

                //right thing
                bool eligible = true;
                for (auto &x : removals)
                {
                    if (x.first.first == i && x.first.second == j && x.second == 'R')
                    {
                        eligible = false;
                    }
                }
                if (eligible)
                {
                    index_pairs.pb({cnt, cnt + NUM_ROWS});
                }
            }
            cnt++;
        }
    }

    // unsigned int indices[index_pairs.size() * 2];

    // for (int i = 0; i < index_pairs.size(); i++)
    // {
    //     debug(i);
    //     indices[2 * i] = index_pairs[i].first;
    //     indices[2 * i + 1] = index_pairs[i].second;
    //     debug(indices[2 * i]);
    //     debug(indices[2 * i + 1]);
    //     part;
    // }
    this->index_pairs = index_pairs;
    this->grid_tracking = grid_tracking;
    this->num_elements_in_indices = 2 * index_pairs.size();
    this->object = create_maze_vao(GL_TRIANGLES, NUM_ROWS * NUM_COLS, grid_points, GL_FILL, index_pairs);

    //now, its time to create vertex arrays and related attribute arrays
}
void Maze_obj::draw()
{
    // Matrices.model = glm::mat4(1.0f);
    // // glm::mat4 translate = glm::translate(this->position); // glTranslatef
    // // glm::mat4 rotate = glm::rotate((float)(this->rotation * M_PI / 180.0f), glm::vec3(1, 1, 1));
    // // // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // // // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    // // Matrices.model *= (translate * rotate);
    // glm::mat4 MVP = VP * Matrices.model;
    // GLCall(glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]));
    GLCall(draw_maze(this->object, this->num_elements_in_indices));
}
