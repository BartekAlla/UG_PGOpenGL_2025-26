#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
    float angleX = 0.0f;
    float angleY = 0.0f;
    float distance = 0.0f;

    glm::mat4 GetViewMatrix() const
    {
        glm::mat4 view(1.0f);
        // zoom
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f - distance));
        // obrót kamery
        view = glm::rotate(view, angleX, glm::vec3(1, 0, 0));
        view = glm::rotate(view, angleY, glm::vec3(0, 1, 0));

        return view;
    }
    // Klawiatura
    void ProcessKeyboard(int key)
    {
        const float rotSpeed = 0.1f;
        const float zoomSpeed = 0.2f;

        switch (key)
        {
        case GLFW_KEY_W: angleX += rotSpeed; break;
        case GLFW_KEY_S: angleX -= rotSpeed; break;
        case GLFW_KEY_A: angleY -= rotSpeed; break;
        case GLFW_KEY_D: angleY += rotSpeed; break;
        case GLFW_KEY_KP_ADD:      distance -= zoomSpeed; break;
        case GLFW_KEY_KP_SUBTRACT: distance += zoomSpeed; break;
        }
    }

    // Scroll
    void ProcessScroll(double yoffset)
    {
        distance -= (float)yoffset;
    }

    // Mysz
    void ProcessMouseDrag(double dx, double dy, int windowWidth, int windowHeight)
    {
        angleX += dy / (float)windowWidth;
        angleY += dx / (float)windowHeight;
    }
};
