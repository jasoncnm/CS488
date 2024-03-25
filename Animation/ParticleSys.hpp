#if !defined(PARTICLESYS_H)

// #include <vector>
#include <glm/glm.hpp>

struct Particle {
    glm::mat4 trans;
    glm::vec3 pos, dpos;
};

#define PARTICLESYS_H
#endif
