// Termm--Fall 2020

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

const static float minhit = 0.05f;
const static float tol = 0.0001f;

Mesh::Mesh( const std::string& fname )
        : m_vertices()
        , m_faces()
{
    type = PrimitiveType::MESH;

    std::string code;
    double vx, vy, vz;
    size_t s1, s2, s3;

    std::ifstream ifs( fname.c_str() );
    while( ifs >> code ) {
        if( code == "v" ) {
            ifs >> vx >> vy >> vz;
            m_vertices.push_back( glm::vec3( vx, vy, vz ) );
        } else if( code == "f" ) {
            ifs >> s1 >> s2 >> s3;
            m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
        }
    }
}

#if 1
bool Mesh::Hit(const glm::vec3 & e, const glm::vec3 & d) {
    HitRecord rec;
    rec.t = FLT_MAX;
    return Mesh::Hit(e, d, rec);
    
}

bool Mesh::Hit(const glm::vec3 & e, const glm::vec3 & dir, HitRecord & record) {
    bool hit = false;
    glm::vec3 A, B, C, R, P0, P1, P2;
    float t, beta, gamma, d, d1, d2, d3;
    for (Triangle face : m_faces) {
        P0 = m_vertices[face.v1];
        P1 = m_vertices[face.v2];
        P2 = m_vertices[face.v3];
        // TODO: triangle intersection!!!!
        R = P0 - e;
        A = P0 - P1;
        B = P0 - P2;
        C = dir;
        d = glm::determinant(glm::mat3(A, B, C));
        d3 = glm::determinant(glm::mat3(A, B, R));
        t = d3 / d;
        if (t > record.t || t < minhit) continue;
        
        d2 = glm::determinant(glm::mat3(A, R, C));
        gamma = d2 / d;
        
        if (gamma < 0 || gamma > 1) continue;

        d1 = glm::determinant(glm::mat3(R, B, C));
        beta = d1 / d;
        if (beta < 0 || beta > (1 - gamma)) continue;

        hit = true;
        record.t = t;
        record.hit_point = e + t * dir;
        record.normal = glm::normalize(glm::cross(P1 - P0, P2 - P1));
    }
    return hit;
}
#endif 
std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
    const MeshVertex& v = mesh.m_verts[idx];
    out << glm::to_string( v.m_position );
    if( mesh.m_have_norm ) {
      out << " / " << glm::to_string( v.m_normal );
    }
    if( mesh.m_have_uv ) {
      out << " / " << glm::to_string( v.m_uv );
    }
  }

*/
  out << "}";
  return out;
}
