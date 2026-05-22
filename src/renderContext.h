//
// Created by Riley on 5/16/2026.
//

#ifndef LEARN_OPENGL_RENDERCONTEXT_H
#define LEARN_OPENGL_RENDERCONTEXT_H

#pragma once

enum class RenderPass {Main, Shadow, Shadow2D, ShadowCube};

class RenderContext {
private:
    static RenderPass currentPass;

public:
    // Only renderer can set this
    static void setPass(RenderPass pass) { currentPass = pass; }

    // Mesh is only permitted to access this
    static RenderPass getPass() { return currentPass; }
};


#endif //LEARN_OPENGL_RENDERCONTEXT_H