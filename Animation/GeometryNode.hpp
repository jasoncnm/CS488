// Termm--Fall 2020

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "ParticleSys.hpp"

class GeometryNode : public SceneNode {
public:
    GeometryNode( const std::string & name, Primitive *prim, 
        Material *mat = nullptr );

    GeometryNode( const std::string & name, Primitive *prim, 
                  uint particle_count, Material *mat = nullptr );

    virtual ~GeometryNode();

    void setMaterial( Material *material );
    void SetParticles(uint particle_count, uint frame_num);

    Material *m_material;
    Primitive *m_primitive;

    bool is_particle = false;
    uint particle_count;
    Particle particles[64];
    uint frame_num;
    
};
