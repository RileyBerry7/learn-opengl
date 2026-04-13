//
// Created by Riley on 4/10/2026.
//

#ifndef LEARN_OPENGL_MODEL_H
#define LEARN_OPENGL_MODEL_H

//-----------------------------------------------------------------------

#include <memory>

//std
#include <string>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"

//======================================================================================

class Mesh {
public:
    // OpenGL Attributes
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    //
    std::unique_ptr<VAO> vao;
    std::unique_ptr<VBO> vbo;
    std::unique_ptr<EBO> ebo;

    int      vertex_count = 0;
    int      index_count  = 0;
    int      stride       = 8;

    // TinyObjLoader Attributes
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    std::string filename;

    ~Mesh() {
        vao->Delete();
        vbo->Delete();
        ebo->Delete();
    }
    Mesh(std::string filePath) {
        loadModel(filePath);

        // Initialize VAO, VBO, EBO
        vao = std::make_unique<VAO>();
        vao->Bind();
        vbo = std::make_unique<VBO>(vertices.data(), sizeof(GLfloat) * vertex_count * stride); // 8 = stride length
        ebo = std::make_unique<EBO>(indices.data(), sizeof(unsigned int)  * index_count);

        // Links VBO1 to VAO
        vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, stride * sizeof(float), (void*)0);;
        vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, stride * sizeof(float), (void*)(3*sizeof(float)));;
        vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, stride * sizeof(float), (void*)(6*sizeof(float)));;

        vao->Unbind();
        vbo->Unbind();

        // Vertex Count & Index Count
        // std::cout << "\nVertex Count: " << mesh.vertex_count << std::endl;
        // std::cout << "Index Count: "    << mesh.index_count  << std::endl;
    }

    void loadModel(std::string filePath){
        // Load Mesh
        filename = filePath;

        bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());
        if (!warn.empty()) std::cout << "WARN: " << warn << "\n";
        if (!err.empty())  std::cout << "ERR: " << err << "\n";
        if (!success) {
            std::cerr << "Failed to load OBJ\n";
            // return;
            // return 1;
        }
        vertices.reserve(shapes.size() * 1000); // optional optimization

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {

                // -------------------------
                // POSITION (x, y, z)
                // -------------------------
                float vx = attrib.vertices[3 * index.vertex_index + 0];
                float vy = attrib.vertices[3 * index.vertex_index + 1];
                float vz = attrib.vertices[3 * index.vertex_index + 2];

                // -------------------------
                // COLOR (r, g, b)
                // OBJ usually doesn't have this → default white
                // -------------------------
                float r = 1.0f;
                float g = 1.0f;
                float b = 1.0f;

                // -------------------------
                // UV (u, v)
                // -------------------------
                float u = 0.0f;
                float v = 0.0f;

                if (!attrib.texcoords.empty() && index.texcoord_index >= 0) {
                    u = attrib.texcoords[2 * index.texcoord_index + 0];
                    v = attrib.texcoords[2 * index.texcoord_index + 1];
                }

                // -------------------------
                // PUSH INTERLEAVED VERTEX
                // -------------------------
                vertices.push_back(vx);
                vertices.push_back(vy);
                vertices.push_back(vz);

                vertices.push_back(r);
                vertices.push_back(g);
                vertices.push_back(b);

                vertices.push_back(u);
                vertices.push_back(v);
            }
        }
    //------------------------------------------------------
    for (unsigned int i = 0; i < vertices.size()/8; i++) {
        indices.push_back(i);
    }
    //------------------------------------------------------

    vertex_count = vertices.size() / 8;
    index_count  = indices.size();
    }

};

#endif //LEARN_OPENGL_MODEL_H