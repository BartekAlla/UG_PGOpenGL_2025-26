#pragma once
#include <glm/glm.hpp>

struct Material
{
    glm::vec3 ambient  = glm::vec3(1.0f);
    glm::vec3 diffuse  = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
    float shininess    = 1.0f;
};
