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

    //==================================================================================================================
    // Private Memory Framework
    //==================================================================================================================
    void invalidate() {
        cleanup(); // Guard against memory leaks if called sequentially

        // 1. Generate the Framebuffer Object via Modern DSA
        glCreateFramebuffers(1, &m_FboId);

        // 2. Generate and allocate the underlying Color Texture via Modern DSA
        glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorTextureId);
        glTextureStorage2D(m_ColorTextureId, 1, GL_RGBA8, m_Width, m_Height);
        glTextureParameteri(m_ColorTextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_ColorTextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Attach the color texture directly to the Framebuffer layout
        glNamedFramebufferTexture(m_FboId, GL_COLOR_ATTACHMENT0, m_ColorTextureId, 0);

        // 3. Generate and allocate the standard depth/stencil backing renderbuffer object
        glCreateRenderbuffers(1, &m_DepthStencilRboId);
        glNamedRenderbufferStorage(m_DepthStencilRboId, GL_DEPTH24_STENCIL8, m_Width, m_Height);
        
        // Attach the renderbuffer directly to the Framebuffer layout
        glNamedFramebufferRenderbuffer(m_FboId, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilRboId);

        // 4. Verify structural consistency before finishing
        if (glCheckNamedFramebufferStatus(m_FboId, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Error: Framebuffer creation failed or incomplete structure layout!" << std::endl;
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

public:
    //==================================================================================================================
    // Lifecycle Operations
    //==================================================================================================================
    FBO(int width, int height) : m_Width(width), m_Height(height) {
        invalidate();
    }

    virtual ~FBO() {
        cleanup();
    }

    // Prevent copy mechanics
    FBO(const FBO&) = delete;
    FBO& operator=(const FBO&) = delete;

    // Safe move semantics
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

            other.m_FboId = 0;
            other.m_ColorTextureId = 0;
            other.m_DepthStencilRboId = 0;
            other.m_Width = 0;
            other.m_Height = 0;
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
    ShadowFBO(int width, int height) : FBO(width, height) {}

    void attachDepthArray(unsigned int textureArrayId) {
        glNamedFramebufferTexture(getId(), GL_DEPTH_ATTACHMENT, textureArrayId, 0);
        glNamedFramebufferDrawBuffer(getId(), GL_NONE);
        glNamedFramebufferReadBuffer(getId(), GL_NONE);
    }

    void attachCubemapArray(unsigned int cubemapArrayId) {
        glNamedFramebufferTexture(getId(), GL_DEPTH_ATTACHMENT, cubemapArrayId, 0);
        glNamedFramebufferDrawBuffer(getId(), GL_NONE);
        glNamedFramebufferReadBuffer(getId(), GL_NONE);
    }
};

#endif //LEARN_OPENGL_FBO_H