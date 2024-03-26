// Termm--Fall 2020

#include <iostream>

#include "Primitive.hpp"
#include "polyroots.hpp"

const static float tol = 0.0001f;
const static double PI = 3.14159265;
static bool once = true;

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

TexturePlane::~TexturePlane() {
    delete texture;
}


TextureSphere::~TextureSphere() {
    delete texture;
}


bool TexturePlane::Hit(const glm::vec3 & e, const glm::vec3 & d, float epi) {

    HitRecord record;
    record.t = FLT_MAX;
    return TexturePlane::Hit(e, d, record, epi);
}


static bool InBound(glm::vec3 pos, glm::vec3 normal) {
    bool result = false;
    if (normal.x == 1) {
        result = (pos.z >= 0 && pos.z <= 1 &&
                  pos.y >= 0 && pos.y <= 1);
    } else if (normal.y == 1) {
        result = (pos.x >= 0 && pos.x <= 1 &&
                  pos.z >= 0 && pos.z <= 1);
    } else if (normal.z == 1) {
        result = (pos.x >= 0 && pos.x <= 1 &&
                  pos.y >= 0 && pos.y <= 1);    
    }
    return result;
}

static float truncate_float_errors(float val) {
    float result = val;
    if (result <= tol && result >= -tol) {
        result = 0;
    }
    return result;
}

glm::vec3 MapTexture(float u, float v, const Image * texture) {

    glm::vec3 result(0);
    float di, dj, up, vp;
    int i, j;
    glm::vec3 C00, C01, C10, C11;
 
    uint tw = texture->width();
    uint th = texture->height();
    // std::cout << tw << " " << th << " " << std::endl;
    v = 1.0f - v;

    di = ((float)tw - 1.0) * u;
    dj = ((float)th - 1.0) * v;
    i = (int)di;
    j = (int)dj;
    up = di - i;
    vp = dj - j;

    vp = glm::clamp((double)vp, 0.0, 1.0);
    up = glm::clamp((double)up, 0.0, 1.0);

    C00 = glm::vec3((*texture)(i, j, 0), (*texture)(i, j, 1), (*texture)(i, j, 2));
    C01 = glm::vec3((*texture)(i, j + 1, 0), (*texture)(i, j + 1, 1), (*texture)(i, j + 1, 2));
    C10 = glm::vec3((*texture)(i+1, j, 0), (*texture)(i+1, j, 1), (*texture)(i+1, j, 2));
    C11 = glm::vec3((*texture)(i+1, j+1, 0), (*texture)(i+1, j+1, 1), (*texture)(i+1, j+1, 2));

#if 0
    if (i == 408 && j == 223) {
        glm::vec3 c = 255.0 * C00;
        std::cout << "C00 " << c.x << " " << c.y << " " << c.z << std::endl;
        double R = (*texture)(408, 223, 0);
        double G = (*texture)(408, 223, 1);
        double B = (*texture)(408, 223, 2);
        std::cout << "colour at (408, 223): " << R << " " << G << " " << B << std::endl;        
    }
#endif    

#if 1
    result = C00 * (1.0f - up) * (1.0f - vp) + C01 * (1.0 - up) * vp
        + C10 * up * (1 - vp) + C11 * up * vp;
#else
    result = C00;
#endif
    return result;
}

glm::vec3 TexturePlane::ApplyTexture(glm::vec3 pos) {
    glm::vec3 result(0);
        
#if 0
    double R = 255.0 * (*texture)(16, 29, 0);
    double G = 255.0 * (*texture)(16, 29, 1);
    double B = 255.0 * (*texture)(16, 29, 2);
    if (once) {        
        once = false;
        std::cout << "Texture_image_width " << tw << " height "
                  << th << std::endl;
        std::cout << "colour at (16,29): " << R << " " << G << " " << B << std::endl;
    }
#endif    

    float u, v;
    
    if (normal.x == 1) {
        u = pos.z;
        v = pos.y;
    } else if (normal.y == 1) {
        u = pos.x;
        v = pos.z;
    } else if (normal.z == 1) {
        u = pos.x;
        v = pos.y;
    }

    result = MapTexture(u, v, texture);
    
    return result;
}

bool TexturePlane::Hit(const glm::vec3 & e, const glm::vec3 & d, HitRecord & record, float epi) {
    bool hit = false;
    
    //NOTE: Front
    glm::vec3 n = normal;
    glm::vec3 p = glm::vec3(0);
    
    float denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        float nom = dot(p - e, n);
        float t = nom / denom;
        if (t < record.t && t > epi) {
            glm::vec3 pos = e + t * d;
            
            if (InBound(pos, normal)) {
                hit = true;
                record.hit_point = pos;
                record.t = t;
                record.normal = n;
            }
        }
    }
    return hit;
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
                record.hit_point = p + record.normal * epi;
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

bool TextureSphere::Hit(const glm::vec3 & e, const glm::vec3 & d, float epi) {
    HitRecord record;
    record.t = FLT_MAX;
    return TextureSphere::Hit(e, d, record, epi);
}

bool TextureSphere::Hit(const glm::vec3 & e, const glm::vec3 & d, HitRecord & record, float epi) {

    Sphere s;
    bool hit = s.Hit(e, d, record, epi);
    return hit;
    
}

glm::vec3 TextureSphere::ApplyTexture(glm::vec3 pos) {

    glm::vec3 n = normalize(pos);
    
    float u = atan(n.x/n.z) / PI + 0.5f;
    float v = 0.5f * n.y + 0.5f;
    
    glm::vec3 result = MapTexture(u, v, texture);

    return result;
}
