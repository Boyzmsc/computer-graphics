#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

class Material
{
public:
    Material() 
    :   ambient(0.1f),
        diffuse(1.0f),
        specular(1.0f),
        shininess(5.0f) {}
    Material(
        const glm::vec3& _ambient,
        const glm::vec3& _diffuse,
        const glm::vec3& _specular,
        float     _shininess
    ) : ambient(_ambient), diffuse(_diffuse), specular(_specular), shininess(_shininess) {}

public:
    std::string name;           // material name

    glm::vec3   ambient;        // ambient reflection coefficients k_a (r, g, b)
    glm::vec3   diffuse;        // diffuse reflection coefficients k_d (r, g, b)
    glm::vec3   specular;       // specular reflection coefficients k_s (r, g, b)
    float       shininess;      // shininess term
};
