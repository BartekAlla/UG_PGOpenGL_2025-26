#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

// Bibliteka GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class CMesh
{
public:

    // ----------------------------------------------------------
    // Tworzenie modelu z pliku OBJ + konfiguracja VAO
    // ----------------------------------------------------------
    bool CreateFromOBJ(const char* Filename)
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        if (!loadOBJ(Filename, vertices, uvs, normals))
            return false;

        vertexCount = vertices.size();

        glGenVertexArrays(1, &idVAO);
        glBindVertexArray(idVAO);

        // Współrzędne
        glGenBuffers(1, &idVBO_coords);
        glBindBuffer(GL_ARRAY_BUFFER, idVBO_coords);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        // Normale
        glGenBuffers(1, &idVBO_normals);
        glBindBuffer(GL_ARRAY_BUFFER, idVBO_normals);
        glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        return true;
    }

    // ----------------------------------------------------------
    // Rysowanie obiektu
    // ----------------------------------------------------------
    void Draw()
    {
        glBindVertexArray(idVAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);
    }

    // ----------------------------------------------------------
    // Transformacje modelu
    // ----------------------------------------------------------
    void SetPosition(const glm::vec3& pos)
    {
        Position = pos; RebuildModelMatrix();
    }

    void SetRotation(const glm::vec3& rot)
    {
        Rotation = rot; RebuildModelMatrix();
    }

    const glm::mat4& GetModelMatrix() const { return matModel; }

    // ----------------------------------------------------------
    // Zwolnienie pamięci
    // ----------------------------------------------------------
    void Clean()
    {
        glDeleteBuffers(1, &idVBO_coords);
        glDeleteBuffers(1, &idVBO_normals);
        glDeleteVertexArrays(1, &idVAO);
    }


private:

    void RebuildModelMatrix()
    {
        matModel = glm::mat4(1.0);
        matModel = glm::translate(matModel, Position);
        matModel = glm::rotate(matModel, Rotation.x, glm::vec3(1,0,0));
        matModel = glm::rotate(matModel, Rotation.y, glm::vec3(0,1,0));
        matModel = glm::rotate(matModel, Rotation.z, glm::vec3(0,0,1));
    }


private:
    GLuint idVAO = 0;
    GLuint idVBO_coords = 0;
    GLuint idVBO_normals = 0;

    GLuint vertexCount = 0;

    glm::vec3 Position = glm::vec3(0);
    glm::vec3 Rotation = glm::vec3(0);
    glm::mat4 matModel = glm::mat4(1.0);
};
