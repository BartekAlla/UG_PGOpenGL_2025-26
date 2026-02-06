#pragma once
#include <array>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../stb_image.h"

class CSkyBox
{
public:
    CSkyBox() = default;
    ~CSkyBox() { Destroy(); }

    CSkyBox(const CSkyBox&) = delete;
    CSkyBox& operator=(const CSkyBox&) = delete;

    bool InitFromDirectory(const std::string& dir,
                           const std::string& vsPath,
                           const std::string& fsPath)
    {
        std::array<std::string, 6> faces = {
            dir + "/posx.jpg",
            dir + "/negx.jpg",
            dir + "/posy.jpg",
            dir + "/negy.jpg",
            dir + "/posz.jpg",
            dir + "/negz.jpg"
        };
        return InitFromFaces(faces, vsPath, fsPath);
    }


    bool InitFromFaces(const std::array<std::string, 6>& faces,
                       const std::string& vsPath,
                       const std::string& fsPath)
    {
        Destroy();

        if (!CreateProgramFromFiles(vsPath, fsPath))
            return false;

        CreateCubeGeometry();

        if (!LoadCubemap(faces))
            return false;

        return true;
    }

    void Render(const glm::mat4& matProj, const glm::mat4& matView) const
    {
        if (!m_program || !m_vao || !m_cubemap)
            return;
        
        glm::mat4 viewNoTrans = glm::mat4(glm::mat3(matView));

        GLint oldDepthFunc = 0;
        glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);

        GLboolean oldDepthMask;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &oldDepthMask);

        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        glUseProgram(m_program);
        glUniform1f(glGetUniformLocation(m_program, "scale"), 40.0f);
        glUniformMatrix4fv(glGetUniformLocation(m_program, "matProj"),
                           1, GL_FALSE, glm::value_ptr(matProj));
        glUniformMatrix4fv(glGetUniformLocation(m_program, "matView"),
                           1, GL_FALSE, glm::value_ptr(viewNoTrans));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
        glUniform1i(glGetUniformLocation(m_program, "tex_skybox"), 0);

        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        glUseProgram(0);

        // restore
        glDepthMask(oldDepthMask);
        glDepthFunc(oldDepthFunc);
    }

private:
    GLuint m_program = 0;
    GLuint m_vao = 0, m_vbo = 0, m_ebo = 0;
    GLuint m_cubemap = 0;

private:
    static std::string ReadTextFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
            return {};

        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    static GLuint CompileShader(GLenum type, const std::string& src, const char* debugName)
    {
        GLuint sh = glCreateShader(type);
        const char* cstr = src.c_str();
        glShaderSource(sh, 1, &cstr, nullptr);
        glCompileShader(sh);

        GLint ok = 0;
        glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            char log[2048];
            glGetShaderInfoLog(sh, sizeof(log), nullptr, log);
            std::cerr << "[SkyBox] Shader compile failed (" << debugName << "):\n" << log << "\n";
            glDeleteShader(sh);
            return 0;
        }
        return sh;
    }

    bool CreateProgramFromFiles(const std::string& vsPath, const std::string& fsPath)
    {
        std::string vsSrc = ReadTextFile(vsPath);
        std::string fsSrc = ReadTextFile(fsPath);

        if (vsSrc.empty() || fsSrc.empty())
        {
            std::cerr << "[SkyBox] Can't read shader files:\n"
                      << "VS: " << vsPath << "\nFS: " << fsPath << "\n";
            return false;
        }

        GLuint vs = CompileShader(GL_VERTEX_SHADER, vsSrc, vsPath.c_str());
        if (!vs) return false;

        GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSrc, fsPath.c_str());
        if (!fs) { glDeleteShader(vs); return false; }

        m_program = glCreateProgram();
        glAttachShader(m_program, vs);
        glAttachShader(m_program, fs);
        glLinkProgram(m_program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        GLint ok = 0;
        glGetProgramiv(m_program, GL_LINK_STATUS, &ok);
        if (!ok)
        {
            char log[2048];
            glGetProgramInfoLog(m_program, sizeof(log), nullptr, log);
            std::cerr << "[SkyBox] Program link failed:\n" << log << "\n";
            glDeleteProgram(m_program);
            m_program = 0;
            return false;
        }
        return true;
    }

    void CreateCubeGeometry()
    {
        // sześcian (8 wierzch.)
        const GLfloat pos[8 * 3] = {
             1,  1,  1,   -1,  1,  1,   -1, -1,  1,    1, -1,  1,
             1,  1, -1,   -1,  1, -1,   -1, -1, -1,    1, -1, -1
        };

        const GLuint idx[36] = {
            5,0,1,  5,4,0,
            2,0,3,  2,1,0,
            7,0,4,  7,3,0,
            3,6,2,  3,7,6,
            1,2,6,  1,6,5,
            4,5,6,  4,6,7
        };

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    bool LoadCubemap(const std::array<std::string, 6>& faces)
    {
        static const GLenum targets[6] = {
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
        };

        glGenTextures(1, &m_cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

        stbi_set_flip_vertically_on_load(false);

        for (int i = 0; i < 6; i++)
        {
            int w, h, n;
            unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &n, 0);
            if (!data)
            {
                std::cerr << "[SkyBox] Can't load: " << faces[i] << "\n";
                stbi_set_flip_vertically_on_load(true);
                return false;
            }

            GLenum format = (n == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(targets[i], 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        stbi_set_flip_vertically_on_load(true);
        return true;
    }

    void Destroy()
    {
        if (m_cubemap) { glDeleteTextures(1, &m_cubemap); m_cubemap = 0; }
        if (m_ebo) { glDeleteBuffers(1, &m_ebo); m_ebo = 0; }
        if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
        if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
        if (m_program) { glDeleteProgram(m_program); m_program = 0; }
    }
};
