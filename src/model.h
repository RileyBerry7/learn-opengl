//
// Created by Riley on 4/10/2026.
//

#ifndef LEARN_OPENGL_MODEL_H
#define LEARN_OPENGL_MODEL_H

//-----------------------------------------------------------------------

//std
#include <string>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"

//======================================================================================

class Model {
public:
    // OpenGL Attributes
    std::vector<float>        vertexBuffer;
    std::vector<unsigned int> indexBuffer;

    int      vertex_count = 0;
    int      index_count  = 0;
    int      stride       = 8;

    // TinyObjLoader Attributes
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    std::string filename;;

    Model(std::string filePath) {

        filename = filePath;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());
    if (!warn.empty()) std::cout << "WARN: " << warn << "\n";
    if (!err.empty())  std::cout << "ERR: " << err << "\n";
    if (!success) {
        std::cerr << "Failed to load OBJ\n";
        // return;
        // return 1;
    }
    vertexBuffer.reserve(shapes.size() * 1000); // optional optimization

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
            vertexBuffer.push_back(vx);
            vertexBuffer.push_back(vy);
            vertexBuffer.push_back(vz);

            vertexBuffer.push_back(r);
            vertexBuffer.push_back(g);
            vertexBuffer.push_back(b);

            vertexBuffer.push_back(u);
            vertexBuffer.push_back(v);
        }
    }
    //------------------------------------------------------
    for (unsigned int i = 0; i < vertexBuffer.size()/8; i++) {
        indexBuffer.push_back(i);
    }
    //------------------------------------------------------

    vertex_count = vertexBuffer.size() / 8;
    index_count  = indexBuffer.size();
    }
};

#endif //LEARN_OPENGL_MODEL_H