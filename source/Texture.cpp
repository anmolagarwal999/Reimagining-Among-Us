#include "Texture.h"

#include "stb_image.h"
using namespace std;

//https://learnopengl.com/Getting-started/Textures
Texture::Texture(const string &path, int choice )
    : m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
    /*
    Makes the texture image upside down as OpenGl starts origin at the bottom left and not at the top left
    */
    stbi_set_flip_vertically_on_load(1);

    /*
    The function first takes as input the location of an image file.
    It then expects you to give three ints as its second, third and fourth argument that stb_image.h will
    fill with the resulting image's width, height and number of color channels.
    We need the image's width and height for generating textures later on.
    */
    cout << "path is " << path << endl;
    m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 0);
    if (m_LocalBuffer)
    {
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        std::cout << "1) Failed to load texture" << std::endl;

        ASSERT(false);
    }
    //The glGenTextures function first takes as input how many textures we want to generate and stores them in a unsigned int array given as its second argument (in our case just a single unsigned int). Just like other objects we need to bind it so any subsequent texture commands will configure the currently bound texture:
    GLCall(glGenTextures(1, &m_RendererID));
    debug(m_RendererID);
    //Now that the texture is bound, we can start generating a texture using the previously loaded image data.
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID)); // Bind without slot selection

    //// set the texture wrapping/filtering options (on the currently bound texture object)

    // GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    // GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    // GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    // GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // set the texture wrapping parameters
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    // set texture filtering parameters
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    //----------------------------------------------------

    // GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
    if (choice == 0)
    {
        GLCall(glTexImage2D(
            GL_TEXTURE_2D,
            0,        //The second argument specifies the mipmap level for which we want to create a texture for if you want to set each mipmap level manually, but we'll leave it at the base level which is 0.
            GL_RGBA,  //store the texture with RGB values
            m_Width,  //set width of texture
            m_Height, //set height of texture
            0,        //The next argument should always be 0 (some legacy stuff).
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            m_LocalBuffer //The last argument is the actual image data.
            ));
    }
    else
    {
         GLCall(glTexImage2D(
            GL_TEXTURE_2D,
            0,        //The second argument specifies the mipmap level for which we want to create a texture for if you want to set each mipmap level manually, but we'll leave it at the base level which is 0.
            GL_RGB,  //store the texture with RGB values
            m_Width,  //set width of texture
            m_Height, //set height of texture
            0,        //The next argument should always be 0 (some legacy stuff).
            GL_RGB,
            GL_UNSIGNED_BYTE,
            m_LocalBuffer //The last argument is the actual image data.
            ));
    }

    GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    // Unbind();

    // if (m_LocalBuffer)
    //     stbi_image_free(m_LocalBuffer);
};

Texture::~Texture()
{
    // cout << "DESTRUCTOR CALLED\n";
    // ASSERT(false);
    // GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
    // debug(slot);
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    // debug(m_RendererID);
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
