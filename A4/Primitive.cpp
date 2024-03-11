// Termm--Fall 2020

#include "Primitive.hpp"
#include "polyroots.hpp"

const static float minhit = 0.05f;
const static float tol = 0.0001f;

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


bool NonhierBox::Hit(const glm::vec3 & e, const glm::vec3 & d) {
    glm::vec3 normal, hit_point;
    float min_t = FLT_MAX;
    return NonhierBox::Hit(e, d, normal, hit_point, min_t);
}

bool NonhierBox::Hit(const glm::vec3 & e, const glm::vec3 & d, glm::vec3 & normal,
                     glm::vec3 & hit_point, float & min_t) {
    bool hit = false;
    double size = m_size;
    glm::vec3 n, p, pos;
    float denom, nom, t, left, right, top, bottom, front, back;

    left = m_pos.x;
    right = left + size;
    bottom = m_pos.y;
    top = bottom + size;
    back = m_pos.z;
    front = back + size;
    
    //NOTE: Front
    n = glm::vec3(0, 0, 1);
    p = m_pos + glm::vec3(0,0, size);
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t < min_t && t > minhit) {
            pos = e + t * d;
            if (pos.x >= left && pos.x <= right &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                min_t = t;
                hit_point = pos;
                normal = n;
            }
        }
    }
     

    //NOTE: Back
    n = glm::vec3(0, 0, -1);
    p = m_pos;
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t < min_t && t > minhit) {
            pos = e + t * d;
            if (pos.x >= left && pos.x <= right &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                min_t = t;
                hit_point = pos;
                normal = n;
            }
        }
    }
    
    //NOTE: Left
    n = glm::vec3(-1, 0, 0);
    p = m_pos;
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t < min_t && t > minhit) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                min_t = t;
                hit_point = pos;
                normal = n;
            }
        }
    }


    //NOTE: Right
    n = glm::vec3(1, 0, 0);
    p = m_pos + glm::vec3(size, 0,0);
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t < min_t && t > minhit) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                min_t = t;
                hit_point = pos;
                normal = n;
            }
        }
    }

    //NOTE: Top
    n = glm::vec3(0, 1, 0);
    p = m_pos + glm::vec3(0, size, 0);
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t < min_t && t > minhit) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.x >= left && pos.x <= right) {
                hit = true;
                min_t = t;
                hit_point = pos;
                normal = n;
            }
        }
    }

    //NOTE: Bottom
    n = glm::vec3(0, -1, 0);
    p = m_pos;
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t < min_t && t > minhit) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.x >= left && pos.x <= right) {
                hit = true;
                min_t = t;
                hit_point = pos;
                normal = n;
            }
        }
    }

    return hit;
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
