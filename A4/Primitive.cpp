// Termm--Fall 2020

#include "Primitive.hpp"
#include "polyroots.hpp"

const static float minhit = 0.01f;


Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::Hit(const glm::vec3 & e, const glm::vec3 & d, glm::vec3 & normal,
                        glm::vec3 & hit_point, float & min_t) {
    bool hit = false;
    double A = glm::dot(d,d);
    double B = 2.0f * glm::dot(d, e - m_pos);
    double C = glm::dot(e - m_pos, e - m_pos) - (m_radius * m_radius);
    double roots[2];
    size_t n_roots;
    n_roots = quadraticRoots( A, B, C, roots);
    if (n_roots > 0 && n_roots <= 2) {
        for(int i = 0; i < n_roots; i++) {
            if (roots[i] < min_t && roots[i] > minhit) {
                hit = true;
                min_t = roots[i];
                glm::vec3 p = e + (min_t * d);
                normal = glm::normalize(2.0f * (p - m_pos));
                hit_point = p;
            }
        }
    }
    return hit;
}

bool NonhierSphere::Hit(const glm::vec3 & e, const glm::vec3 & d) {
    bool hit = false;
    double A = glm::dot(d,d);
    double B = glm::dot(2.0f * d, e - m_pos);
    double C = glm::dot(e - m_pos, e - m_pos) - (m_radius * m_radius);
    double roots[2];
    size_t n_roots;
    n_roots = quadraticRoots( A, B, C, roots);
    if (n_roots > 0 && n_roots <= 2) {
        if (roots[1] > minhit || roots[2] > minhit) {
            hit = true;
        }
    }
    return hit;
}




NonhierBox::~NonhierBox()
{
}
