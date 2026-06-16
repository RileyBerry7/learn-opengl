//
// Created by Riley on 6/15/2026.
//
#include "betterTexture.h"

//======================================================================================================================
BetterTexture::BetterTexture(GLenum target) {

    glCreateTextures(target, 1, &id);

}
//----------------------------------------------------------------------------------------------------------------------


//======================================================================================================================
void BetterTexture::create2DArray(int width, int height, int layers, GLenum internalFormat) {
    glTextureStorage3D(id, 1, internalFormat, width, height, layers);
}
//----------------------------------------------------------------------------------------------------------------------


//======================================================================================================================
void BetterTexture::setFilter(GLenum minFilter, GLenum magFilter) {
    // Standard shadow map filtering
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, minFilter);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, magFilter);
}

//======================================================================================================================
void BetterTexture::setWrap(GLenum s, GLenum t, GLenum r) {
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, s);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, t);
}

//======================================================================================================================
void BetterTexture::setBorderColor(float r, float g, float b, float a) {
    // Set border color for shadow maps to prevent artifacts outside frustum
    float borderColor[] = { 1.0f, r, g, b };
    glTextureParameterfv(id, GL_TEXTURE_BORDER_COLOR, borderColor);
}


void BetterTexture::bind() const {
    switch (target) {
        case GL_TEXTURE_2D: glBindTexture(GL_TEXTURE_2D, 0);
            break;
        // case GL_TEXTURE_2D_ARRAY:
    }
}
void BetterTexture::unbind() const {

}
void BetterTexture::bindUnit(GLuint unit) const {
    glBindTextureUnit(unit, id);

}

int BetterTexture::getWidth() const {
    return width;
}
int BetterTexture::getHeight() const {
   return height;
}
