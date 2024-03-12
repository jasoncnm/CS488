
#pragma once
#include <glm/ext.hpp>
#include "PhongMaterial.hpp"


struct HitRecord {
    float t;
    glm::vec3 hit_point, normal, kd, ks;
    double shininess;
};
