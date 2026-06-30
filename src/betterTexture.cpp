//
// Created by Riley on 6/15/2026.
//
#include "betterTexture.h"

unsigned char* getImage(const char* fileName, int& widthImg, int& heightImg, int& numColorCh) {
    // Get Image
    stbi_set_flip_vertically_on_load(true);

    unsigned char* image = stbi_load(fileName, &widthImg, &heightImg, &numColorCh, 0);
    if (!image) {
        std::cerr << "Failed to load texture: " << fileName << "\n";
    }
    return image;
}

//======================================================================================================================
BetterTexture::BetterTexture(GLenum target) {
    glCreateTextures(target, 1, &id);
    this->target = target;
}

//======================================================================================================================
void BetterTexture::load2D(const std::string& path) {
    setFilter(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
    setWrap(GL_REPEAT, GL_REPEAT);

    if (path != "None") {
        unsigned char* image = getImage(path.c_str(), width, height, numColorCh);

        // Crucial check: Only proceed if the image actually loaded successfully
        if (image != nullptr) {
            GLenum internalFormat = GL_RGB8;
            GLenum uploadFormat = GL_RGB;

            // Automatically deduce the correct channel configuration
            if (numColorCh == 1) {
                internalFormat = GL_R8;    // 1-channel grayscale (Displacement maps)
                uploadFormat = GL_RED;
            } else if (numColorCh == 3) {
                internalFormat = GL_RGB8;  // 3-channel color (Normal maps / standard JPEGs)
                uploadFormat = GL_RGB;
            } else if (numColorCh == 4) {
                internalFormat = GL_RGBA8; // 4-channel color (PNGs with transparency)
                uploadFormat = GL_RGBA;
            }

            // Calculate standard mipmap levels safely based on texture dimensions
            GLsizei levels = 1 + std::floor(std::log2(std::max(width, height)));

            // Prevent driver crashes if image width/height rows are not perfectly aligned to 4 bytes
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // Allocate immutable storage using DSA
            glTextureStorage2D(id, levels, internalFormat, width, height);

            // Safely upload data using the matched color channel counts
            glTextureSubImage2D(id, 0, 0, 0, width, height, uploadFormat, GL_UNSIGNED_BYTE, image);

            glGenerateTextureMipmap(id);
            stbi_image_free(image);
        }
    }
}

//======================================================================================================================
void BetterTexture::loadCubemap(const std::string& fileName) {
    std::vector<std::string> faceNames = {"right", "left", "top", "bottom", "front", "back"};
    std::vector<std::string> faces(6);

    std::string basePath = "resources/cubemaps/" + fileName + '/';
    std::vector<std::string> extensions = {".jpg", ".jpeg", ".png", ".bmp"};

    // 1. Dynamically find the file extensions that actually exist
    for (unsigned int i = 0; i < faceNames.size(); i++) {
        bool fileFound = false;
        for (const std::string& ext : extensions) {
            std::string testPath = basePath + faceNames[i] + ext;
            if (fs::exists(testPath)) {
                faces[i] = testPath;
                fileFound = true;
                break;
            }
        }
        if (!fileFound) {
            std::cout << "Error: Missing asset for face: " << faceNames[i] << " in " << basePath << std::endl;
            return;
        }
    }

    int nrChannels;
    stbi_set_flip_vertically_on_load(false);

    // 2. Load the first face to determine dimensions
    unsigned char *firstFaceData = stbi_load(faces[0].c_str(), &width, &height, &nrChannels, 4);
    if (!firstFaceData) {
        std::cout << "Cubemap failed to load first face: " << faces[0] << std::endl;
        return;
    }

    // 3. Allocate immutable memory using Modern DSA
    glTextureStorage2D(id, 1, GL_RGBA8, width, height);

    // 4. Upload layer 0
    glTextureSubImage3D(id, 0, 0, 0, 0, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, firstFaceData);
    stbi_image_free(firstFaceData);

    // 5. Load and upload the remaining 5 layers
    for (unsigned int i = 1; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 4);
        if (data) {
            glTextureSubImage3D(id, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap failed to load face: " << faces[i] << std::endl;
        }
    }

    // 6. Apply parameters using your class DSA methods
    setFilter(GL_LINEAR, GL_LINEAR);
    setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}//----------------------------------------------------------------------------------------------------------------------


//======================================================================================================================
void BetterTexture::create2D(int width, int height, GLenum internalFormat) {
glTextureStorage2D(id, 1, internalFormat, width, height);
}

//======================================================================================================================
void BetterTexture::create2DArray(int width, int height, int layers, GLenum internalFormat) {
    glTextureStorage3D(id, 1, internalFormat, width, height, layers);
}

//======================================================================================================================
void BetterTexture::createCubemapArray(int width, int height, int cubeCount, GLenum internalFormat) {
    glTextureStorage3D(id, 1, internalFormat, width, height, cubeCount * 6);
}
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

//======================================================================================================================
void BetterTexture::bind() const {
    glBindTexture(target, id);
}
//======================================================================================================================
void BetterTexture::unbind() const {
    glBindTexture(target, 0);
}
//======================================================================================================================
void BetterTexture::bindUnit(GLuint unit) const {
    glBindTextureUnit(unit, id);
}
//======================================================================================================================

int BetterTexture::getWidth() const {
    return width;
}
//======================================================================================================================
int BetterTexture::getHeight() const {
   return height;
}
