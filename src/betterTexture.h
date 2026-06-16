//
// Created by Riley on 6/15/2026.
//
#ifndef LEARN_OPENGL_BETTERTEXTURE_H
#define LEARN_OPENGL_BETTERTEXTURE_H
//----------------------------------------------------------------------------------------------------------------------
#include "glad/glad.h"
#include <string>
#include <vector>
//======================================================================================================================
class BetterTexture {

private:
    GLuint id     = 0;
    GLenum target = GL_TEXTURE_2D; // default

    int width  = 0;
    int height = 0;
    int depth  = 0;
//----------------------------------------------------------------------------------------------------------------------

public:
    BetterTexture();              // Default Constructor
    BetterTexture(GLenum target); // Parameterized Constructor

    // ~BetterTexture(); // Destructor

    BetterTexture(const BetterTexture&) = delete;            // Copy Constructor (deleted)
    BetterTexture& operator=(const BetterTexture&) = delete; // Copy Assignment (deleted)

    BetterTexture(BetterTexture&& other) noexcept;            // Move Constructor
    BetterTexture& operator=(BetterTexture&& other) noexcept; // Assignment Operator

    //----------------------------------------------------------------------------------------------------------------------
    // CREATION

    void load2D(const std::string& path);
    void loadCubemap(const std::vector<std::string>& faces);

    void create2D(int width, int height, GLenum internalFormat);
    void create2DArray(int width, int height, int layers, GLenum internalFormat);

    void createCubemap(int width, int height, GLenum internalFormat);
    void createCubemapArray(int width, int height, int cubeCount, GLenum internalFormat);

    //----------------------------------------------------------------------------------------------------------------------
    // PARAMETERS

    void setFilter(GLenum minFilter, GLenum magFilter);
    void setWrap(GLenum s, GLenum t, GLenum r = GL_REPEAT);
    void setBorderColor(float r, float g, float b, float a);

    //----------------------------------------------------------------------------------------------------------------------
    // BINDING

    void bind() const;
    void unbind() const;
    void bindUnit(GLuint unit) const;

    //----------------------------------------------------------------------------------------------------------------------
    // ACCESSORS

    int getWidth() const;
    int getHeight() const;
    // TODO: Temporary Delete
    GLuint getID() const {
        return id;
    }
};
//======================================================================================================================
#endif //LEARN_OPENGL_BETTERTEXTURE_H