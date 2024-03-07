// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

enum PrimitiveType { SPHERE, CUBE, NHSPHERE, NHBOX, };

class Primitive {
public:
    PrimitiveType type;
    virtual ~Primitive();
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
    NonhierSphere(const glm::vec3& pos, double radius)
            : m_pos(pos), m_radius(radius)
    {
        type = PrimitiveType::NHSPHERE;
    }

    bool Hit(const glm::vec3 & e, const glm::vec3 & d,
             glm::vec3 & normal, glm::vec3 & hit_point, float & min_t);

    bool Hit(const glm::vec3 & e, const glm::vec3 & d);
    
    virtual ~NonhierSphere();

private:
    glm::vec3 m_pos;
    double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};
