#pragma once
#include "../imgui.h"
#include "../imgui_impl_glfw.h"
#include "../imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

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

    float monkeyAngle = 0.0f;
    bool enablePointLight = true;
    // 0 = Phong, 1 = Blinn-Phong
    int lightingModel = 0;  
};
