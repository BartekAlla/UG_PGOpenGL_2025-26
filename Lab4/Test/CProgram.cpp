#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

// Bibliteka GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class CProgram
{
public:

    // ----------------------------------------------------------
    // Tworzenie programu i ładowanie shaderów
    // ----------------------------------------------------------
    bool Init(const char* VertexFile, const char* FragmentFile)
    {
        idProgram = glCreateProgram();
        GLuint vs = LoadShader(GL_VERTEX_SHADER, VertexFile);
        GLuint fs = LoadShader(GL_FRAGMENT_SHADER, FragmentFile);

        glAttachShader(idProgram, vs);
        glAttachShader(idProgram, fs);
        LinkAndValidateProgram(idProgram);

        glDeleteShader(vs);
        glDeleteShader(fs);

        return true;
    }

    // ----------------------------------------------------------
    // Czyszczenie zasobów
    // ----------------------------------------------------------
    void Clean()
    {
        if (idProgram)
            glDeleteProgram(idProgram);
    }

    // ----------------------------------------------------------
    // Aktywowanie i dezaktywowanie programu
    // ----------------------------------------------------------
    void Use()   { glUseProgram(idProgram); }
    void UnUse() { glUseProgram(0); }

    // ----------------------------------------------------------
    // Przekazywanie uniformów
    // ----------------------------------------------------------
    void SetFloat(const char* Name, float v)
    {
        glUniform1f(glGetUniformLocation(idProgram, Name), v);
    }

    void SetInt(const char* Name, int v)
    {
        glUniform1i(glGetUniformLocation(idProgram, Name), v);
    }

    void SetMat4(const char* Name, const glm::mat4& M)
    {
        glUniformMatrix4fv(glGetUniformLocation(idProgram, Name), 1, GL_FALSE, glm::value_ptr(M));
    }

    GLuint getId() const { return idProgram; }

private:
    GLuint idProgram = 0;
};
