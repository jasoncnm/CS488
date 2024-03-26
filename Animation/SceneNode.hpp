// Termm--Fall 2020

#pragma once

#include "Material.hpp"
#include "ParticleSys.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>

enum class NodeType {
    SceneNode,
    GeometryNode,
    JointNode
};

class SceneNode {
public:
    SceneNode(const std::string & name);

    SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
    int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);

    //-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);


    void InitParticles(uint particle_count, uint frame_num);
    void UpdateParticles(uint frame_num);
    
    friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<SceneNode*> children;

    NodeType m_nodeType;
    std::string m_name;
    unsigned int m_nodeId;

    bool is_particle = false;
    uint particle_count, frame_num;
//    Particle particles[64];
    

private:
    // The number of SceneNode instances.
    static unsigned int nodeInstanceCount;
};
