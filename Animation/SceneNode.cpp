// Termm--Fall 2020

#include "SceneNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
    m_nodeType(NodeType::SceneNode),
    trans(mat4()),
    invtrans(mat4()),
    m_nodeId(nodeInstanceCount++)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
    : m_nodeType(other.m_nodeType),
      m_name(other.m_name),
      trans(other.trans),
      invtrans(other.invtrans)
{
    for(SceneNode * child : other.children) {
        this->children.push_front(new SceneNode(*child));
    }
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
    for(SceneNode * child : children) {
        delete child;
    }
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
    trans = m;
    invtrans = glm::inverse(m);
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
    return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
    return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
    children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
    children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
    vec3 rot_axis;

    switch (axis) {
        case 'x':
            rot_axis = vec3(1,0,0);
            break;
        case 'y':
            rot_axis = vec3(0,1,0);
            break;
        case 'z':
            rot_axis = vec3(0,0,1);
            break;
        default:
            break;
    }
    mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
    set_transform( rot_matrix * trans );
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
    set_transform( glm::scale(amount) * trans );
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
    set_transform( glm::translate(amount) * trans );
}


//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
    return nodeInstanceCount;
}


//---------------------------------------------------------------------------------------
void SceneNode::reset_transform() {
    set_transform( glm::mat4(1.) );
}



//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

    //os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
    switch (node.m_nodeType) {
        case NodeType::SceneNode:
            os << "SceneNode";
            break;
        case NodeType::GeometryNode:
            os << "GeometryNode";
            break;
        case NodeType::JointNode:
            os << "JointNode";
            break;
    }
    os << ":[";

    os << "name:" << node.m_name << ", ";
    os << "id:" << node.m_nodeId;

    os << "]\n";
    return os;
}




#if 0
//---------------------------------------------------------------------------------------
void SceneNode::InitParticles(uint particle_count, uint frame_num) {

    this->is_particle = true;
    this->particle_count = particle_count;
    this->frame_num = frame_num;

    for (uint i = 0; i < particle_count; i++) {
        // TODO: Initial Random Position for each particle
        // TODO: What is the ideal speed of the particle? should it be randomized?
        vec3 dpos(0, 0.2, 0);
        GeometryNode * gnode = new GeometryNode( m_name, new Cube() );
        
    }
    
}

//---------------------------------------------------------------------------------------
void SceneNode::UpdateParticles(uint frame_num);

#endif
