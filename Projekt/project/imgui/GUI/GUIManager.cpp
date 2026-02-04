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

    ImGui::End();
}

void GUIManager::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
