#include "InstancedMesh.hpp"
#include <cstdlib>
#include <cmath>
#include "../stb_image.h"

CMultipleMesh::~CMultipleMesh()
{
    if (VBO_instance_posScale) glDeleteBuffers(1, &VBO_instance_posScale);
    if (VBO_instance_rotPhase) glDeleteBuffers(1, &VBO_instance_rotPhase);
    if (textureID) glDeleteTextures(1, &textureID);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
}
static float frand(float a, float b) {
    return a + (b - a) * (rand() / (float)RAND_MAX);
}

bool CMultipleMesh::InitQuad()
{
    // 2 trójkąty, atrybuty: pos (loc0) + uv (loc2)
    float quad[] = {
        // x, y, z,   u, v
        -0.5f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f, 0.0f,  1.0f, 0.0f,
         0.5f, 1.0f, 0.0f,  1.0f, 1.0f,

        -0.5f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 1.0f, 0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Instance buffers (posScale -> loc3, rotPhase -> loc4)
    glGenBuffers(1, &VBO_instance_posScale);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_instance_posScale);
    glBufferData(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glGenBuffers(1, &VBO_instance_rotPhase);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_instance_rotPhase);
    glBufferData(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);
    return true;
}

bool CMultipleMesh::LoadTexture(const char* path)
{
    int w, h, ch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &w, &h, &ch, 0);
    if (!data) return false;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = (ch == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return true;
}

void CMultipleMesh::CreateInstances(size_t count, float areaHalfSize, float yMin, float yMax)
{
    m_instances.clear();
    m_instances.reserve(count);

    for (size_t i = 0; i < count; i++)
    {
        Instance ins;
        ins.pos = glm::vec3(
            frand(-areaHalfSize, areaHalfSize),
            frand(yMin, yMax),
            frand(-areaHalfSize, areaHalfSize)
        );

        ins.vel = glm::vec3(
            frand(-1.0f, 1.0f),
            frand(-0.2f, 0.2f),
            frand(-1.0f, 1.0f)
        );

        ins.scale = frand(0.08f, 0.18f);
        ins.rot = frand(0.0f, 6.28318f);
        ins.rotSpeed = frand(-3.0f, 3.0f);

        ins.phase = frand(0.0f, 6.28318f);
        ins.phaseSpeed = frand(1.0f, 4.0f);

        m_instances.push_back(ins);
    }

    UploadInstanceBuffer();
}

void CMultipleMesh::Update(float dt, float areaHalfSize, float yMin, float yMax)
{
    for (size_t i = 0; i < m_activeCount; i++)
    {
        auto& m = m_instances[i];
        // pseudo losowy drift (lekko zmienia kierunek)
        m.vel.x += 0.3f * frand(-1.0f, 1.0f) * dt;
        m.vel.z += 0.3f * frand(-1.0f, 1.0f) * dt;

        // ogranicz prędkość
        float maxV = 2.0f;
        m.vel.x = std::max(-maxV, std::min(maxV, m.vel.x));
        m.vel.z = std::max(-maxV, std::min(maxV, m.vel.z));

        m.pos += m.vel * dt;

        // wrap po planszy
        if (m.pos.x > areaHalfSize) m.pos.x = -areaHalfSize;
        if (m.pos.x < -areaHalfSize) m.pos.x = areaHalfSize;
        if (m.pos.z > areaHalfSize) m.pos.z = -areaHalfSize;
        if (m.pos.z < -areaHalfSize) m.pos.z = areaHalfSize;

        // jeśli spadnie za nisko / za wysoko – odbij
        if (m.pos.y < yMin) { m.pos.y = yMin; m.vel.y = std::abs(m.vel.y); }
        if (m.pos.y > yMax) { m.pos.y = yMax; m.vel.y = -std::abs(m.vel.y); }

        m.rot += m.rotSpeed * dt;
        m.phase += m.phaseSpeed * dt;
    }

    UploadActiveToGPU();
}

void CMultipleMesh::UploadInstanceBuffer()
{
    std::vector<glm::vec4> posScale(m_instances.size());
    std::vector<glm::vec4> rotPhase(m_instances.size());

    for (size_t i = 0; i < m_instances.size(); i++)
    {
        posScale[i] = glm::vec4(m_instances[i].pos, m_instances[i].scale);
        rotPhase[i] = glm::vec4(m_instances[i].rot, m_instances[i].phase, 0.0f, 0.0f);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO_instance_posScale);
    glBufferData(GL_ARRAY_BUFFER, posScale.size() * sizeof(glm::vec4), posScale.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_instance_rotPhase);
    glBufferData(GL_ARRAY_BUFFER, rotPhase.size() * sizeof(glm::vec4), rotPhase.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CMultipleMesh::DrawInstanced(GLuint programID, const glm::mat4& matProj, const glm::mat4& matView)
{
    glUseProgram(programID);

    glUniformMatrix4fv(glGetUniformLocation(programID, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
    glUniformMatrix4fv(glGetUniformLocation(programID, "matView"), 1, GL_FALSE, glm::value_ptr(matView));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(programID, "textureSampler"), 0);

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei)m_activeCount);
    glBindVertexArray(0);

    glUseProgram(0);
}
bool CMultipleMesh::InitQuadInstanced(size_t maxInstances)
{
    m_capacity = maxInstances;
    m_activeCount = 0;
    m_instances.reserve(m_capacity);

    // --- quad VAO/VBO (jak miałeś w InitQuad) ---
    float quad[] = {
        -0.5f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f, 0.0f,  1.0f, 0.0f,
         0.5f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 1.0f, 0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(2);

    // --- TERAZ: instance VBO alokujemy na MAX ---
    glGenBuffers(1, &VBO_instance_posScale);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_instance_posScale);
    glBufferData(GL_ARRAY_BUFFER, m_capacity*sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glGenBuffers(1, &VBO_instance_rotPhase);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_instance_rotPhase);
    glBufferData(GL_ARRAY_BUFFER, m_capacity*sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);
    return true;
}


void CMultipleMesh::SetActiveCount(size_t desired, glm::vec3 center, float halfSize, float yMin, float yMax)
{
    if (desired > m_capacity) desired = m_capacity;

    // jeśli zwiększamy: dopisz nowe instancje do vectora
    while (m_instances.size() < desired)
    {
        Instance ins;
        ins.pos = glm::vec3(
            center.x + frand(-halfSize, halfSize),
            frand(yMin, yMax),
            center.z + frand(-halfSize, halfSize)
        );
        ins.vel = glm::vec3(frand(-1, 1), frand(-0.2f, 0.2f), frand(-1, 1));
        ins.scale = frand(0.06f, 0.16f);
        ins.rot = frand(0.0f, 6.28318f);
        ins.rotSpeed = frand(-3.0f, 3.0f);
        ins.phase = frand(0.0f, 6.28318f);
        ins.phaseSpeed = frand(1.0f, 4.0f);

        m_instances.push_back(ins);
    }

    // aktywna liczba do rysowania
    m_activeCount = desired;

    // wrzuć na GPU dane 0..activeCount-1
    UploadActiveToGPU();
}
void CMultipleMesh::UploadActiveToGPU()
{
    std::vector<glm::vec4> posScale(m_activeCount);
    std::vector<glm::vec4> rotPhase(m_activeCount);

    for (size_t i = 0; i < m_activeCount; i++)
    {
        posScale[i] = glm::vec4(m_instances[i].pos, m_instances[i].scale);
        rotPhase[i] = glm::vec4(m_instances[i].rot, m_instances[i].phase, 0, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO_instance_posScale);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_activeCount*sizeof(glm::vec4), posScale.data());

    glBindBuffer(GL_ARRAY_BUFFER, VBO_instance_rotPhase);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_activeCount*sizeof(glm::vec4), rotPhase.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int CMultipleMesh::RemoveInSphere(const glm::vec3& center, float radius)
{
    if (m_activeCount == 0) return 0;

    const float r2 = radius * radius;
    int removed = 0;

    size_t i = 0;
    while (i < m_activeCount)
    {
        glm::vec3 d = m_instances[i].pos - center;
        // możesz ignorować wysokość (Y), jeśli chcesz tylko po XZ:
        // d.y = 0.0f;

        if (glm::dot(d, d) <= r2)
        {
            // swap z ostatnim aktywnym
            m_instances[i] = m_instances[m_activeCount - 1];
            m_activeCount--;
            removed++;
            // NIE zwiększaj i, bo nowy element na i też trzeba sprawdzić
        }
        else
        {
            i++;
        }
    }

    if (removed > 0)
        UploadActiveToGPU();

    return removed;
}
int CMultipleMesh::RemoveColliding(const glm::vec3& center, float radius, bool xzOnly)
{
    if (m_activeCount == 0) return 0;

    float r2 = radius * radius;
    int removed = 0;

    size_t i = 0;
    while (i < m_activeCount)
    {
        glm::vec3 d = m_instances[i].pos - center;
        if (xzOnly) d.y = 0.0f;

        if (glm::dot(d, d) <= r2)
        {
            // SWAP-REMOVE: podmień trafionego z ostatnim aktywnym
            m_instances[i] = m_instances[m_activeCount - 1];
            m_activeCount--;
            removed++;
            // nie zwiększaj i (nowy element na i też trzeba sprawdzić)
        }
        else
        {
            i++;
        }
    }

    if (removed > 0)
        UploadActiveToGPU(); // ważne: upload po zmianie aktywnych

    return removed;
}

