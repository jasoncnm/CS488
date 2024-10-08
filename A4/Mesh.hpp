// Termm--Fall 2020

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"
#include "BoundBox.hpp"

// Use this #define to selectively compile your code to render the
// bounding boxes around your mesh objects. Uncomment this option
// to turn it on.
//NOTE: (VM) When compile with -O2, enable this does not render nonhier.lua properly
//      (gl33) nonhier.lua works with -O2 on strange ??? 
// #define RENDER_BOUNDING_VOLUMES

struct Triangle
{
    size_t v1;
    size_t v2;
    size_t v3;

    Triangle( size_t pv1, size_t pv2, size_t pv3 )
            : v1( pv1 )
            , v2( pv2 )
            , v3( pv3 )
    {}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
    Mesh( const std::string& fname );
    bool Hit(const glm::vec3 & e, const glm::vec3 & d, HitRecord & record, float epi);
    bool Hit(const glm::vec3 & e, const glm::vec3 & d, float epi);
    
private:
    std::vector<glm::vec3> m_vertices;
    std::vector<Triangle> m_faces;
    BoundBox bound_box;
    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
