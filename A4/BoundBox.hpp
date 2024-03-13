#pragma once

#include "Hit_Record.hpp"

struct BoundBox {
    glm::vec3 min, max;
    bool Hit(const glm::vec3 & e, const glm::vec3 & d, float epi);
    bool Hit(const glm::vec3 & e, const glm::vec3 & d, HitRecord & record, float epi);

};
