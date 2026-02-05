#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class CMultipleMesh
{
public:
    struct Instance
    {
        glm::vec3 pos;
        glm::vec3 vel;
        float scale;
        float rot;
        float rotSpeed;
        float phase;
        float phaseSpeed;
    };

    CMultipleMesh() = default;
    ~CMultipleMesh();

    bool InitQuad(); // tworzy VAO kwadratu
    bool LoadTexture(const char* path); // tak jak w Mesh
    void CreateInstances(size_t count, float areaHalfSize, float yMin, float yMax);
    void Update(float dt, float areaHalfSize, float yMin, float yMax);
    void DrawInstanced(GLuint programID, const glm::mat4& matProj, const glm::mat4& matView);
    bool InitQuadInstanced(size_t maxInstances);
    void SetActiveCount(size_t desired, glm::vec3 center, float halfSize, float yMin, float yMax);
    int RemoveColliding(const glm::vec3& center, float radius, bool xzOnly = true);
    size_t ActiveCount() const { return m_activeCount; }
    int RemoveInSphere(const glm::vec3& center, float radius);
    size_t Count() const { return m_instances.size(); }

private:
    void UploadInstanceBuffer(); // wrzuca iPosScale + iRotPhase
    void UploadActiveToGPU();

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint textureID = 0;

    GLuint VBO_instance_posScale = 0;
    GLuint VBO_instance_rotPhase = 0;

    size_t m_capacity = 0;  
    size_t m_activeCount = 0;
    std::vector<Instance> m_instances;
};
