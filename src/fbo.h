//
// Created by Riley on 6/17/2026.
//
#ifndef LEARN_OPENGL_FBO_H
#define LEARN_OPENGL_FBO_H

#include <glad/glad.h>
#include <iostream>
#include <utility>

class FBO {
private:
    unsigned int m_FboId = 0;
    unsigned int m_ColorTextureId = 0;
    unsigned int m_DepthStencilRboId = 0;

    int m_Width = 0;
    int m_Height = 0;
    bool m_IsShadowMap = false; // Added flag to distinguish shadow-only buffers

    //==================================================================================================================
    // Private Memory Framework
    //==================================================================================================================
    void invalidate() {
        cleanup();

        // 1. Generate the Framebuffer Object via Modern DSA
        glCreateFramebuffers(1, &m_FboId);

        // 2. Only build standard color and renderbuffers if this is NOT a shadow map pass
        if (!m_IsShadowMap) {
            // Generate and allocate the underlying Color Texture via Modern DSA
            glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorTextureId);
            glTextureStorage2D(m_ColorTextureId, 1, GL_RGBA8, m_Width, m_Height);
            glTextureParameteri(m_ColorTextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_ColorTextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Attach the color texture directly to the Framebuffer layout
            glNamedFramebufferTexture(m_FboId, GL_COLOR_ATTACHMENT0, m_ColorTextureId, 0);

            // Generate and allocate the standard depth/stencil backing renderbuffer object
            glCreateRenderbuffers(1, &m_DepthStencilRboId);
            glNamedRenderbufferStorage(m_DepthStencilRboId, GL_DEPTH24_STENCIL8, m_Width, m_Height);

            // Attach the renderbuffer directly to the Framebuffer layout
            glNamedFramebufferRenderbuffer(m_FboId, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilRboId);

            // Verify structural consistency for color framebuffers
            if (glCheckNamedFramebufferStatus(m_FboId, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "Error: Standard Framebuffer creation failed!" << std::endl;
            }
        }
    }

    void cleanup() {
        if (m_FboId) {
            glDeleteFramebuffers(1, &m_FboId);
            m_FboId = 0;
        }
        if (m_ColorTextureId) {
            glDeleteTextures(1, &m_ColorTextureId);
            m_ColorTextureId = 0;
        }
        if (m_DepthStencilRboId) {
            glDeleteRenderbuffers(1, &m_DepthStencilRboId);
            m_DepthStencilRboId = 0;
        }
    }

protected:
    // Protected constructor allows the Shadow subclass to flag itself before allocating
    FBO(int width, int height, bool isShadowMap) : m_Width(width), m_Height(height), m_IsShadowMap(isShadowMap) {
        invalidate();
    }

public:
    //==================================================================================================================
    // Lifecycle Operations
    //==================================================================================================================
    FBO(int width, int height) : m_Width(width), m_Height(height), m_IsShadowMap(false) {
        invalidate();
    }

    virtual ~FBO() {
        cleanup();
    }

    FBO(const FBO&) = delete;
    FBO& operator=(const FBO&) = delete;

    FBO(FBO&& other) noexcept {
        *this = std::move(other);
    }

    FBO& operator=(FBO&& other) noexcept {
        if (this != &other) {
            cleanup();

            m_FboId = other.m_FboId;
            m_ColorTextureId = other.m_ColorTextureId;
            m_DepthStencilRboId = other.m_DepthStencilRboId;
            m_Width = other.m_Width;
            m_Height = other.m_Height;
            m_IsShadowMap = other.m_IsShadowMap;

            other.m_FboId = 0;
            other.m_ColorTextureId = 0;
            other.m_DepthStencilRboId = 0;
            other.m_Width = 0;
            other.m_Height = 0;
            other.m_IsShadowMap = false;
        }
        return *this;
    }

    //==================================================================================================================
    // Pipeline Controls
    //==================================================================================================================
    void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
        glViewport(0, 0, m_Width, m_Height);
    }

    void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void resize(int width, int height) {
        m_Width = width;
        m_Height = height;
        invalidate();
    }

    //==================================================================================================================
    // Inline Getters
    //==================================================================================================================
    [[nodiscard]] GLuint getId() const { return m_FboId; }
    [[nodiscard]] unsigned int getColorTextureId() const { return m_ColorTextureId; }
    [[nodiscard]] int getWidth() const { return m_Width; }
    [[nodiscard]] int getHeight() const { return m_Height; }
};

//======================================================================================================================
// Derived Shadow Optimization Subclass
//======================================================================================================================
class ShadowFBO : public FBO {
public:
    // Explicitly tells the base constructor to bypass standard color/renderbuffer generation
    ShadowFBO(int width, int height) : FBO(width, height, true) {}

    void attachDepthArray(unsigned int textureArrayId) {
        glNamedFramebufferTexture(getId(), GL_DEPTH_ATTACHMENT, textureArrayId, 0);
        glNamedFramebufferDrawBuffer(getId(), GL_NONE);
        glNamedFramebufferReadBuffer(getId(), GL_NONE);

        // Verify structural consistency for shadows after manual attachment
        if (glCheckNamedFramebufferStatus(getId(), GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Error: Shadow Framebuffer status validation failed!" << std::endl;
        }
    }

    void attachCubemapArray(unsigned int cubemapArrayId) {
        glNamedFramebufferTexture(getId(), GL_DEPTH_ATTACHMENT, cubemapArrayId, 0);
        glNamedFramebufferDrawBuffer(getId(), GL_NONE);
        glNamedFramebufferReadBuffer(getId(), GL_NONE);

        if (glCheckNamedFramebufferStatus(getId(), GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Error: Shadow Cubemap Framebuffer status validation failed!" << std::endl;
        }
    }
};

#endif //LEARN_OPENGL_FBO_H
