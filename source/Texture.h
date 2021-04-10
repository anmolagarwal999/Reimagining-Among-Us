#include "main.h"
#pragma once

// #include "Renderer.h"

/****************************************************************
 
 The first thing we need to do to actually use textures is to load them into our application. Texture images can be stored in dozens of file formats, each with their own structure and ordering of data,
 stb_image.h is a very popular single header image loading library 
 */


class Texture
{
private:
    std::string m_FilePath;       //file path of the texture
    unsigned char *m_LocalBuffer; //local storage for the texture
    int m_Width, m_Height, m_BPP;

public:
    Texture(){};
    Texture(const std::string &path, int choice);
    ~Texture();

    unsigned int m_RendererID;    //renderer id
    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }
};