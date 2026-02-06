#pragma once
#include "../imgui.h"
#include "../imgui_impl_glfw.h"
#include "../imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class GUIManager
{
private:
    GLFWwindow* window;
public:
    GUIManager(GLFWwindow* win);
    ~GUIManager();

    void StartFrame();
    void RenderFrame();
    void EndFrame();
    // Parametry GUI
    bool enablePointLight = true;
    // 0 = Phong, 1 = Blinn-Phong
    int lightingModel = 0;  
    float lightIntensity = 1.0f;
    bool animateLight = false;
    float lightAnimSpeed = 1.0f;
    int skyboxIndex = 0;
    int mosquitoCount = 2000;
    bool enableFog = true;
    int fogMode = 2; // 0 linear, 1 exp, 2 exp2
    float fogDensity = 0.03f;
    float fogStart = 5.0f;
    float fogEnd = 40.0f;
    glm::vec3 fogColor = glm::vec3(0.6f, 0.7f, 0.7f);
};
