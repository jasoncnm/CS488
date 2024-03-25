// Termm--Fall 2020

#include "GeometryNode.hpp"


//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
    const std::string & name, Primitive *prim, Material *mat )
    : SceneNode( name )
    , m_material( mat )
    , m_primitive( prim )
    , is_particle( false )
{
    m_nodeType = NodeType::GeometryNode;
}


//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
    const std::string & name, Primitive *prim, uint particle_count,  Material *mat )
        : SceneNode( name )
        , m_material( mat )
        , m_primitive( prim )
{
    m_nodeType = NodeType::GeometryNode;
    SetParticles(particle_count, 0);
}

//---------------------------------------------------------------------------------------
void GeometryNode::SetParticles(uint particle_count, uint frame_num) {

    this->is_particle = true;
    this->particle_count = particle_count;
    this->frame_num = frame_num;

    for (uint i = 0; i < particle_count; i++) {
        Particle * particle = particles + i;
        // TODO: Initial Random Position for each particle
        // TODO: What is the ideal speed of the particle? should it be randomized?
        particle->trans = glm::mat4(1);
        particle->pos = glm::vec3(0);
        particle->dpos = glm::vec3(0, 0.2, 0);
    }
    
}

//---------------------------------------------------------------------------------------
GeometryNode::~GeometryNode() {
    
}

void GeometryNode::setMaterial( Material *mat )
{
    // Obviously, there's a potential memory leak here.  A good solution
    // would be to use some kind of reference counting, as in the 
    // C++ shared_ptr.  But I'm going to punt on that problem here.
    // Why?  Two reasons:
    // (a) In practice we expect the scene to be constructed exactly
    //     once.  There's no reason to believe that materials will be
    //     repeatedly overwritten in a GeometryNode.
    // (b) A ray tracer is a program in which you compute once, and 
    //     throw away all your data.  A memory leak won't build up and
    //     crash the program.

    m_material = mat;
}
