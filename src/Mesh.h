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
#include "camera.h"
#include "renderContext.h"

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

    std::map<std::string, Shader*> shaderMap;
    std::vector<std::unique_ptr<Material>> materialList;
    std::vector<subMesh> subMeshes;

    // Obj Parse Atrributes
    int vertex_count = 0;
    int index_count  = 0;
    int stride       = 14; // previously 11

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
    Mesh(std::string fileName, std::map<std::string, Shader*> shaderMap) :
    filename(fileName), matDir("resources/models/"), shaderMap(std::move(shaderMap)) {

        // Load model
        loadModel(matDir+fileName, matDir);
        if (vertices.empty()) {
        // Error Check
            std::cerr << "Mesh error: No vertices loaded. Check file path: " << fileName << std::endl;
            return;
        }

        // Load material
        loadMaterial();

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

    void loadModel(std::string objFile, std::string matFile){

        // Tiny OBJ Loader
        bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objFile.c_str(), matFile.c_str());
        if (!warn.empty()) std::cout << "WARN: " << warn << "\n";
        if (!err.empty()) std::cout << "ERR: " << err << "\n";
        if (!success) {
            std::cerr << "Failed to load OBJ: " << objFile << std::endl;
            return;
        }

        vertices.reserve(shapes.size() * 1000); // Reserve space for vertices

    // Grouping indices by material to create subMeshes
    std::map<int, std::vector<unsigned int>> materialToIndices;

    // Map to track unique vertices and avoid massive duplication (Vertex Deduplication)
    // Key is a string of "posIndex_texIndex_normalIndex"
    std::map<std::string, unsigned int> uniqueVertices;

        // SUBMESH LOOP
        for (const auto& shape : shapes) {
            // FACE LOOP
            for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++) {

                // Fetch material ID for this face
                int matID = shape.mesh.material_ids[f];
                if (matID < 0) matID = DEFAULT_MATERIAL_INDEX; // Fallback

                // Pre-fetch positions and UVs for this triangle
                tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
                tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
                tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

                glm::vec3 p0(attrib.vertices[3 * idx0.vertex_index + 0], attrib.vertices[3 * idx0.vertex_index + 1], attrib.vertices[3 * idx0.vertex_index + 2]);
                glm::vec3 p1(attrib.vertices[3 * idx1.vertex_index + 0], attrib.vertices[3 * idx1.vertex_index + 1], attrib.vertices[3 * idx1.vertex_index + 2]);
                glm::vec3 p2(attrib.vertices[3 * idx2.vertex_index + 0], attrib.vertices[3 * idx2.vertex_index + 1], attrib.vertices[3 * idx2.vertex_index + 2]);

                glm::vec2 uv0(0.0f), uv1(0.0f), uv2(0.0f);
                if (!attrib.texcoords.empty()) {
                    if (idx0.texcoord_index >= 0) { uv0.x = attrib.texcoords[2 * idx0.texcoord_index + 0]; uv0.y = attrib.texcoords[2 * idx0.texcoord_index + 1]; }
                    if (idx1.texcoord_index >= 0) { uv1.x = attrib.texcoords[2 * idx1.texcoord_index + 0]; uv1.y = attrib.texcoords[2 * idx1.texcoord_index + 1]; }
                    if (idx2.texcoord_index >= 0) { uv2.x = attrib.texcoords[2 * idx2.texcoord_index + 0]; uv2.y = attrib.texcoords[2 * idx2.texcoord_index + 1]; }
                }

                // Calculate tangent of this face
                glm::vec3 faceTangent = calculateTriangleTangent(p0, p1, p2, uv0, uv1, uv2);

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

                        //------------------------------
                        // TANGENT VECTOR (tx, ty, tz)
                        //------------------------------

                        vertices.push_back(faceTangent.x);
                        vertices.push_back(faceTangent.y);
                        vertices.push_back(faceTangent.z);


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


    void drawBatches(Camera camera, glm::mat4 modelMatrix) {
        // if (materialList.empty()) return; // Safety check: No materials loaded yet

        vao->Bind();
        for (const auto& sm : subMeshes) {
            int idx = sm.materialIndex;
            if (idx < 0 || idx >= (int)materialList.size()) {
                idx = DEFAULT_MATERIAL_INDEX;
            }
            Shader* activeShader = nullptr;
            if (RenderContext::getPass() == RenderPass::Shadow) {
                activeShader = shaderMap[std::string("shadow2d")];
                activeShader->Activate();

            } else {
                // Apply batch material/shader
                activeShader = materialList[idx]->shader;
                activeShader->Activate(); // TODO: Add comparison to avoid necessary activations

                // Apply material
                materialList[idx]->apply();
            }

            // Set common uniforms
            activeShader->setUniform("modelMatrix", modelMatrix);
            camera.Matrix(*activeShader, "camMatrix");
            activeShader->setUniform("viewPos", camera.Position);

            // Draw batch
            glDrawElements(GL_TRIANGLES, sm.count, GL_UNSIGNED_INT, (void*)(uintptr_t)(sm.indexOffset * sizeof(unsigned int)));
        }
        vao->Unbind();
    }

    void loadMaterial() {
        materialList.clear();
        std::string path = "resources/textures/";

        auto difMap   = new BetterTexture(GL_TEXTURE_2D);
        difMap->load2D(path + "missing.png");

        auto specMap  = new BetterTexture(GL_TEXTURE_2D);
        specMap->load2D(path + "missing.png");

        auto defaultMat = std::make_unique<DefaultMaterial>(*shaderMap[std::string("default")], difMap, specMap);
        materialList.push_back(std::move(defaultMat));

        // 1. Convert to material class
        for (const auto& mat : materials) {
            // Emission Handling
            bool has_emission = (mat.emission[0] > 0.0f ||
                                 mat.emission[1] > 0.0f ||
                                 mat.emission[2] > 0.0f);

            // 2. Create your class instance
            Shader* suggestedShader;
            std::unique_ptr<Material> material;
            if (has_emission) {
                suggestedShader = shaderMap[std::string("emissive")];
                auto emisMat = std::make_unique<EmissiveMaterial>(*suggestedShader);
                emisMat->lightColor = glm::vec3(mat.emission[0], mat.emission[1], mat.emission[2]);
                material = std::move(emisMat);
            } else {
                suggestedShader = shaderMap[std::string("default")];
                auto defMat = std::make_unique<DefaultMaterial>(*suggestedShader);

                // 3. Map colors
                defMat->ambient   = glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
                defMat->diffuse   = glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
                defMat->specular  = glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]);
                defMat->shininess = mat.shininess;

                // 4.Map texture maps
                std::string texDir = "resources/textures/";

                if (!mat.diffuse_texname.empty()) {     // DANGLING POINTER !!!!!
                    defMat->diffuseMap = new BetterTexture(GL_TEXTURE_2D);
                    defMat->diffuseMap->load2D(texDir + mat.diffuse_texname);
                }

                if (!mat.specular_texname.empty()) {
                    defMat->specMap = new BetterTexture(GL_TEXTURE_2D);
                    defMat->specMap->load2D(texDir + mat.specular_texname);
                }

                if (!mat.bump_texname.empty()) {
                    defMat->specMap = new BetterTexture(GL_TEXTURE_2D);
                    defMat->specMap->load2D(texDir + mat.bump_texname);
                    defMat->hasNormalMap = true;
                }

                material = std::move(defMat);
            }
            materialList.push_back(std::move(material));
        }
    }


    glm::vec3 calculateTriangleTangent(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2,
                                   const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2)
    {
        glm::vec3 edge1 = p1 - p0;
        glm::vec3 edge2 = p2 - p0;

        float deltaU1 = uv1.x - uv0.x;
        float deltaV1 = uv1.y - uv0.y;
        float deltaU2 = uv2.x - uv0.x;
        float deltaV2 = uv2.y - uv0.y;

        float denom = (deltaU1 * deltaV2 - deltaV1 * deltaU2);
        float fScalar = (std::abs(denom) > 0.00001f) ? 1.0f / denom : 0.0f;

        glm::vec3 faceTangent;
        faceTangent.x = fScalar * (deltaV2 * edge1.x - deltaV1 * edge2.x);
        faceTangent.y = fScalar * (deltaV2 * edge1.y - deltaV1 * edge2.y);
        faceTangent.z = fScalar * (deltaV2 * edge1.z - deltaV1 * edge2.z);

        if (glm::length(faceTangent) > 0.0001f) {
            return glm::normalize(faceTangent);
        }

        return glm::vec3(1.0f, 0.0f, 0.0f); // Fallback axis
    }

};

#endif //LEARN_OPENGL_MODEL_H