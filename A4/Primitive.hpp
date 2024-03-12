// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "Hit_Record.hpp"


enum PrimitiveType { SPHERE, CUBE, NHSPHERE, NHBOX, MESH, };

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

    bool Hit(const glm::vec3 & e, const glm::vec3 & d, HitRecord & record);
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
      type = PrimitiveType::NHBOX;
  }

    bool Hit(const glm::vec3 & e, const glm::vec3 & d, HitRecord & record);
    bool Hit(const glm::vec3 & e, const glm::vec3 & d);
  
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};
