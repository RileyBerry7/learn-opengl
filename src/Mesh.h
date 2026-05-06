#ifndef LEARN_OPENGL_MODEL_H
#define LEARN_OPENGL_MODEL_H

//-----------------------------------------------------------------------

//std
#include <string>
#include <memory>

#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "material.h"

struct subMesh {
    int materialIndex;
    unsigned int indexOffset; // Starting point
    unsigned int count;       // Length
};

#define DEFAULT_MATERIAL_INDEX 0
//======================================================================================

class Mesh {
public:
    // OpenGL Attributes
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    // Composed Objects
    std::unique_ptr<VAO>      vao;
    std::unique_ptr<VBO>      vbo;
    std::unique_ptr<EBO>      ebo;

    Shader* shader;
    std::vector<std::unique_ptr<DefaultMaterial>> materialList;
    std::vector<subMesh> subMeshes;

    // Obj Parse Atrributes
    int vertex_count = 0;
    int index_count  = 0;
    int stride       = 11;

    // TinyObjLoader Attributes
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    std::string filename;
    std::string matDir;

    ~Mesh() {
        // Delete buffer objects
        vao->Delete();
        vbo->Delete();
        ebo->Delete();
        // c++ smart ptrs automatically free memory
    }

    // CONSTRUCTOR
    Mesh(std::string fileName, Shader& shader) :
    filename(fileName), matDir("resources/models/"), shader(&shader) {

        // Load model
        loadModel(matDir+fileName, matDir);
        if (vertices.empty()) {
        // Error Check
            std::cerr << "Mesh error: No vertices loaded. Check file path: " << fileName << std::endl;
            return;
        }

        // Load material
        loadMaterial(shader);

        // Initialize VAO, VBO, EBO
        vao = std::make_unique<VAO>();
        vao->Bind();
        vbo = std::make_unique<VBO>(vertices.data(), sizeof(GLfloat) * vertex_count * stride); // 8 = stride length
        ebo = std::make_unique<EBO>(indices.data(), sizeof(unsigned int)  * index_count);

        // Links VBO1 to VAO
        vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, stride * sizeof(float), (void*)0);
        vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, stride * sizeof(float), (void*)(3*sizeof(float)));
        vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, stride * sizeof(float), (void*)(6*sizeof(float)));
        vao->LinkAttrib(*vbo, 3, 3, GL_FLOAT, stride * sizeof(float), (void*)(8*sizeof(float)));

        vao->Unbind();
        vbo->Unbind();

        // Vertex Count & Index Count
        // std::cout << "\nVertex Count: " << mesh.vertex_count << std::endl;
        // std::cout << "Index Count: "    << mesh.index_count  << std::endl;
    }

void loadModel(std::string objFile, std::string matFile) {
    // Load Mesh
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objFile.c_str(), matFile.c_str());

    if (!warn.empty()) std::cout << "WARN: " << warn << "\n";
    if (!err.empty()) std::cout << "ERR: " << err << "\n";
    if (!success) {
        std::cerr << "Failed to load OBJ: " << objFile << std::endl;
        return;
    }

    vertices.reserve(shapes.size() * 1000);

    // Grouping indices by material to create subMeshes
    std::map<int, std::vector<unsigned int>> materialToIndices;

    // Map to track unique vertices and avoid massive duplication (Vertex Deduplication)
    // Key is a string of "posIndex_texIndex_normalIndex"
    std::map<std::string, unsigned int> uniqueVertices;

    for (const auto& shape : shapes) {
        for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++) {

            // Get material ID for this face (default to 0 if negative)
            int matID = shape.mesh.material_ids[f];

            if (matID < 0) {
                matID = DEFAULT_MATERIAL_INDEX; // use fallback
            }

            for (size_t vi = 0; vi < 3; vi++) {
                tinyobj::index_t index = shape.mesh.indices[3 * f + vi];

                // Create a unique key for this specific vertex combination
                std::string key = std::to_string(index.vertex_index) + "_" +
                                  std::to_string(index.texcoord_index) + "_" +
                                  std::to_string(index.normal_index);

                if (uniqueVertices.find(key) == uniqueVertices.end()) {
                    // Record the index where this new unique vertex will be stored
                    uniqueVertices[key] = static_cast<unsigned int>(vertices.size() / stride);

                    // -------------------------
                    // POSITION (x, y, z)
                    // -------------------------
                    vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
                    vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                    vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

                    // -------------------------
                    // COLOR (r, g, b)
                    // -------------------------
                    vertices.push_back(1.0f);
                    vertices.push_back(1.0f);
                    vertices.push_back(1.0f);

                    // -------------------------
                    // UV (u, v)
                    // -------------------------
                    float u = 0.0f, v = 0.0f;
                    if (!attrib.texcoords.empty() && index.texcoord_index >= 0) {
                        u = attrib.texcoords[2 * index.texcoord_index + 0];
                        v = attrib.texcoords[2 * index.texcoord_index + 1];
                    }
                    vertices.push_back(u);
                    vertices.push_back(v);

                    // -------------------------
                    // NORMAL VECTOR (nx, ny, nz)
                    // -------------------------
                    float nx = 0.0f, ny = 0.0f, nz = 0.0f;
                    if (index.normal_index >= 0) {
                        nx = attrib.normals[3 * index.normal_index + 0];
                        ny = attrib.normals[3 * index.normal_index + 1];
                        nz = attrib.normals[3 * index.normal_index + 2];
                    }
                    vertices.push_back(nx);
                    vertices.push_back(ny);
                    vertices.push_back(nz);
                }

                // Map this triangle's vertex to the correct index in our grouped list
                materialToIndices[matID].push_back(uniqueVertices[key]);
            }
        }
    }

    // FLATTEN: Combine grouped indices into the final index buffer and define subMeshes
    indices.clear();
    subMeshes.clear(); // Ensure list is clean
    for (auto const& [matID, matIndices] : materialToIndices) {
        subMesh batch;
        batch.materialIndex = (matID == -1) ? 0 : matID + 1;
        batch.indexOffset = static_cast<unsigned int>(indices.size());
        batch.count = static_cast<unsigned int>(matIndices.size());

        subMeshes.push_back(batch);
        indices.insert(indices.end(), matIndices.begin(), matIndices.end());
    }

    vertex_count = static_cast<int>(vertices.size() / stride);
    index_count = static_cast<int>(indices.size());
}


    void draw() const {
        // if (materialList.empty()) return; // Safety check: No materials loaded yet

        vao->Bind();
        for (const auto& sm : subMeshes) {
            int idx = sm.materialIndex;
            if (idx < 0 || idx >= (int)materialList.size()) {
                idx = DEFAULT_MATERIAL_INDEX;
            }
            materialList[idx]->apply();
            glDrawElements(GL_TRIANGLES, sm.count, GL_UNSIGNED_INT, (void*)(uintptr_t)(sm.indexOffset * sizeof(unsigned int)));
        }
        vao->Unbind();
        // glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    }

    void loadMaterial(Shader& suggestedShader) {
        materialList.clear();
        std::string path = "resources/textures/";
        GLenum tt     = GL_TEXTURE_2D;
        GLenum pt     = GL_UNSIGNED_BYTE;
        auto difMap   = new Tex(path + "missing.png"  , tt, GL_TEXTURE0, pt); // Diffuse map
        auto specMap  = new Tex(path + "missing.png", tt, GL_TEXTURE1, pt); // Specular map
        auto defaultMat = std::make_unique<DefaultMaterial>(suggestedShader, difMap, specMap);
        materialList.push_back(std::move(defaultMat));

        // 1. Convert to your Class
        for (const auto& mat : materials) {
            // 2. Create your class instance
            auto material = std::make_unique<DefaultMaterial>(suggestedShader);

            // 3. Map colors
            material->ambient   = glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
            material->diffuse   = glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
            material->specular  = glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]);
            material->shininess = mat.shininess;

            // 4.Map texture maps
            std::string texDir = "resources/textures/";
            GLenum tt     = GL_TEXTURE_2D;
            GLenum pt     = GL_UNSIGNED_BYTE;
            if (!mat.diffuse_texname.empty()) {     // DANGLING POINTER !!!!!
                std::string path = texDir + mat.diffuse_texname;
                material->diffuseMap = new Tex(path, tt, GL_TEXTURE0, pt);
            }
            if (!mat.specular_texname.empty()) {
                std::string path = texDir + mat.specular_texname;
                material->specMap = new Tex(path, tt, GL_TEXTURE1, pt);
            }
            materialList.push_back(std::move(material));
        }
    }

};

#endif //LEARN_OPENGL_MODEL_H