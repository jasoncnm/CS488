#include "BoundBox.hpp"


const static float tol = 0.0001f;

bool BoundBox::Hit(const glm::vec3 & e, const glm::vec3 & d, float epi) {
    HitRecord rec;
    rec.t = FLT_MAX;
    return BoundBox::Hit(e, d, rec, epi);
}

bool BoundBox::Hit(const glm::vec3 & e, const glm::vec3 & d, HitRecord & record, float epi) {
    bool hit = false;
    glm::vec3 n, p, pos;
    float denom, nom, t, left, right, top, bottom, front, back;

    left = min.x;
    right = max.x;
    bottom = min.y;
    top = max.y;
    back = min.z;
    front = max.z;
    
    //NOTE: Front
    n = glm::vec3(0, 0, 1);
    p = max;
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t > epi && t < record.t) {
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
    p = min;
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t > epi && t < record.t) {
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
    
    //NOTE: Left
    n = glm::vec3(-1, 0, 0);
    p = min;
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t > epi && t < record.t) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                record.hit_point = pos;
                record.t = t;
                record.normal = n;
            }
        }
    }


    //NOTE: Right
    n = glm::vec3(1, 0, 0);
    p = max;
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t > epi && t < record.t) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.y >= bottom && pos.y <= top) {
                hit = true;
                record.hit_point = pos;
                record.t = t;
                record.normal = n;
            }
        }
    }

    //NOTE: Top
    n = glm::vec3(0, 1, 0);
    p = max;
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t > epi && t < record.t) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.x >= left && pos.x <= right) {
                hit = true;
                record.hit_point = pos;
                record.t = t;
                record.normal = n;
            }
        }
    }

    //NOTE: Bottom
    n = glm::vec3(0, -1, 0);
    p = min;
    denom = dot(d,n);
    if (denom >= tol || denom <= -tol) {
        nom = dot(p - e, n);
        t = nom / denom;
        if (t > epi && t < record.t) {
            pos = e + t * d;
            if (pos.z >= back && pos.z <= front &&
                pos.x >= left && pos.x <= right) {
                hit = true;
                record.hit_point = pos;
                record.t = t;
                record.normal = n;
            }
        }
    }

    return hit;
}
