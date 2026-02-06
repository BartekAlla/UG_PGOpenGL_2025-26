#include "GUIManager.hpp"

GUIManager::GUIManager(GLFWwindow* win)
    : window(win)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

GUIManager::~GUIManager()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUIManager::StartFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUIManager::RenderFrame()
{
    ImGui::Begin("Scene Control");
    ImGui::Text("Animation parameters:");
    ImGui::Checkbox("Enable Point Light", &enablePointLight);
    ImGui::RadioButton("Phong", &lightingModel, 0);
    ImGui::RadioButton("Blinn-Phong", &lightingModel, 1);
    ImGui::SliderFloat(
    "Light intensity",
        &lightIntensity,
        0.0f,
        3.0f
    );
    ImGui::Separator();
    ImGui::Text("Light animation");

    ImGui::Checkbox("Animate light", &animateLight);
    ImGui::SliderFloat("Animation speed", &lightAnimSpeed, 0.1f, 5.0f);
    ImGui::Separator();
    ImGui::Text("Skybox");
    const char* skyNames[] = { "ForestCamp", "River" };
    ImGui::Combo("Active skybox", &this->skyboxIndex, skyNames, IM_ARRAYSIZE(skyNames));
    ImGui::SliderInt("Mosquito count", &mosquitoCount, 0, 100000);
    ImGui::Separator();
    ImGui::Text("Fog");
    ImGui::Checkbox("Enable fog", &enableFog);
    ImGui::RadioButton("Linear", &fogMode, 0);
    ImGui::RadioButton("Exp", &fogMode, 1);
    ImGui::RadioButton("Exp2", &fogMode, 2);
    ImGui::ColorEdit3("Fog color", &fogColor.x);
    ImGui::SliderFloat("Fog density", &fogDensity, 0.0f, 0.2f);
    ImGui::SliderFloat("Fog start", &fogStart, 0.0f, 100.0f);
    ImGui::SliderFloat("Fog end", &fogEnd, 0.0f, 300.0f);
    ImGui::End();
}

void GUIManager::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
