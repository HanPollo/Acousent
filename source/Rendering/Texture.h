#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <glad/glad.h>
#include <iostream>

class Texture {
public:
    // Constructor
    Texture(const char* path, const std::string& directory, GLenum type = GL_TEXTURE_2D);

    // Functions
    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    // Getters
    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }
    inline GLenum GetType() const { return m_Type; }

private:
    // Texture data
    unsigned int m_ID;
    int m_Width, m_Height;
    GLenum m_Type;

    // Functions
    void loadTexture(const char* path, const std::string& directory);
};

#endif
