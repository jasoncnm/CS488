// Termm--Fall 2020

#include "Primitive.hpp"
#include "polyroots.hpp"

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


NonhierBox::~NonhierBox()
{
}


bool NonhierBox::Hit(const glm::vec3 & e, const glm::vec3 & d, float epi) {

    HitRecord record;
    record.t = FLT_MAX;
    return NonhierBox::Hit(e, d, record, epi);
}

bool NonhierBox::Hit(const glm::vec3 & e, const glm::vec3 & d,
                     HitRecord & record, float epi) {
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.x >= left && pos.x <= right &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                record.hit_point = pos;
                record.t = t;
                record.normal = n;
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.x >= left && pos.x <= right &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                record.t = t;
                record.hit_point = pos;
                record.normal = n;
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                record.t = t;
                record.hit_point = pos;
                record.normal = n;
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                record.t = t;
                record.hit_point = pos;
                record.normal = n;
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.x >= left && pos.x <= right) {
                hit = true;
                record.t = t;
                record.hit_point = pos;
                record.normal = n;
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.x >= left && pos.x <= right) {
                hit = true;
                record.t = t;
                record.hit_point = pos;
                record.normal = n;
            }
        }
    }

    return hit;
}

bool NonhierSphere::Hit(const glm::vec3 & e, const glm::vec3 & d,
                        HitRecord & record, float epi) {
    bool hit = false;
    double A = glm::dot(d,d);
    double B = 2.0f * glm::dot(d, e - m_pos);
    double C = glm::dot(e - m_pos, e - m_pos) - (m_radius * m_radius);
    double roots[2];
    size_t n_roots;
    n_roots = quadraticRoots( A, B, C, roots);
    if (n_roots > 0 && n_roots <= 2) {
        for(int i = 0; i < n_roots; i++) {
            if (roots[i] < record.t && roots[i] > epi) {
                hit = true;
                record.t = roots[i];
                glm::vec3 p = e + (record.t * d);
                record.normal = (2.0f * (p - m_pos));
                record.hit_point = p;
            }
        }
    }
    return hit;
}

bool NonhierSphere::Hit(const glm::vec3 & e, const glm::vec3 & d, float epi) {
    bool hit = false;    
    double A = glm::dot(d,d);
    double B = glm::dot(2.0f * d, e - m_pos);
    double C = glm::dot(e - m_pos, e - m_pos) - (m_radius * m_radius);
    double roots[2];
    size_t n_roots;
    n_roots = quadraticRoots( A, B, C, roots);
    if (n_roots > 0 && n_roots <= 2) {
        if (roots[1] > epi || roots[2] > epi) {
            hit = true;
        }
    }
    return hit;
}

bool Sphere::Hit(const glm::vec3 & e, const glm::vec3 & d, float epi) {
    bool hit = false;    
    glm::vec3 m_pos(0,0,0);
    double m_radius = 1.0;
    double A = glm::dot(d,d);
    double B = glm::dot(2.0f * d, e - m_pos);
    double C = glm::dot(e - m_pos, e - m_pos) - (m_radius * m_radius);
    double roots[2];
    size_t n_roots;
    n_roots = quadraticRoots( A, B, C, roots);
    if (n_roots > 0 && n_roots <= 2) {
        if (roots[1] > epi || roots[2] > epi) {
            hit = true;
        }
    }
    return hit;
}

bool Sphere::Hit(const glm::vec3 & e, const glm::vec3 & d,
                 HitRecord & record, float epi) {
    glm::vec3 m_pos(0,0,0);
    double m_radius = 1.0;
    bool hit = false;
    double A = glm::dot(d,d);
    double B = 2.0f * glm::dot(d, e - m_pos);
    double C = glm::dot(e - m_pos, e - m_pos) - (m_radius * m_radius);
    double roots[2];
    size_t n_roots;
    n_roots = quadraticRoots( A, B, C, roots);
    if (n_roots > 0 && n_roots <= 2) {
        for(int i = 0; i < n_roots; i++) {
            if (roots[i] < record.t && roots[i] > epi) {
                hit = true;
                record.t = roots[i];
                glm::vec3 p = e + (record.t * d);
                record.normal = (2.0f * (p - m_pos));
                record.hit_point = p;
            }
        }
    }
    return hit;
}


bool Cube::Hit(const glm::vec3 & e, const glm::vec3 & d, float epi) {
    HitRecord record;
    record.t = FLT_MAX;
    return Cube::Hit(e, d, record, epi);
}


bool Cube::Hit(const glm::vec3 & e, const glm::vec3 & d,
               HitRecord & record, float epi) {
    bool hit = false;
    double size = 1;
    glm::vec3 m_pos(0,0,0);
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.x >= left && pos.x <= right &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                record.hit_point = pos;
                record.t = t;
                record.normal = n;
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.x >= left && pos.x <= right &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                record.t = t;
                record.hit_point = pos;
                record.normal = n;
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                record.t = t;
                record.hit_point = pos;
                record.normal = n;
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                record.t = t;
                record.hit_point = pos;
                record.normal = n;
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.x >= left && pos.x <= right) {
                hit = true;
                record.t = t;
                record.hit_point = pos;
                record.normal = n;
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
        if (t < record.t && t > epi) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.x >= left && pos.x <= right) {
                hit = true;
                record.t = t;
                record.hit_point = pos;
                record.normal = n;
            }
        }
    }

    return hit;
}
