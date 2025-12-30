#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Transform.hpp"

class Mesh
{
public:
    Transform transform;

    // Konstrukcja
    Mesh(const char* objPath);
    Mesh(const std::vector<glm::vec3>& vertices,
         const std::vector<glm::vec2>& uvs);

    ~Mesh();

    void Draw(GLuint shaderProgram) const;
    bool LoadTexture(const char* path);

    GLuint GetVAO() const { return VAO; }
    size_t GetVertexCount() const { return vertexCount; }

private:
    void CreateBuffers(const std::vector<glm::vec3>& vertices,
                       const std::vector<glm::vec3>& normals,
                       const std::vector<glm::vec2>& uvs);

private:
    GLuint VAO = 0;
    GLuint VBO_vertices = 0;
    GLuint VBO_normals  = 0;
    GLuint VBO_uv       = 0;
    GLuint textureID    = 0;

    size_t vertexCount = 0;
};
