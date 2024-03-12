// Termm--Fall 2020

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include "polyroots.hpp"

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

#ifdef RENDER_BOUNDING_VOLUMES
    glm::vec3 center(0);
    double radius = 0;    
    std::ifstream ifs( fname.c_str() );
    while( ifs >> code ) {
        if( code == "v" ) {
            ifs >> vx >> vy >> vz;
            glm::vec3 p(vx, vy, vz);
            double dist = glm::distance(p, glm::vec3(0));
            radius = dist > radius ? dist : radius; 
            m_vertices.push_back( p );
        } else if( code == "f" ) {
            ifs >> s1 >> s2 >> s3;
            m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
        }
    }
    bound_sphere.r = radius;
    
#else

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
    
#endif
}

#ifdef RENDER_BOUNDING_VOLUMES
// TODO: DEBUG THIS !!!!!!!!!!
bool BoundSphere::Hit(const glm::vec3 & e, const glm::vec3 & d) {
    bool hit = false;
    glm::vec3 c(0);
    double A = glm::dot(d,d);
    double B = glm::dot(2.0f * d, e - c);
    double C = glm::dot(e - c, e - c) - (r * r);
    double roots[2];
    size_t n_roots;
    n_roots = quadraticRoots( A, B, C, roots);
    if (n_roots > 0 && n_roots <= 2) {
        if (roots[1] > minhit || roots[2] > minhit) {
            hit = true;
        }
    }
    return hit;
}
#endif


bool Mesh::Hit(const glm::vec3 & e, const glm::vec3 & d) {
    HitRecord rec;
    rec.t = FLT_MAX;
    return Mesh::Hit(e, d, rec);
    
}

bool Mesh::Hit(const glm::vec3 & e, const glm::vec3 & dir,
               HitRecord & record) {

#ifdef RENDER_BOUNDING_VOLUMES
    if (!bound_sphere.Hit(e, dir)) {
        return false;
    }
#endif
    
    bool hit = false;
    glm::vec3 A, B, C, R, P0, P1, P2;
    float t, beta, gamma, d, d1, d2, d3;
    for (Triangle face : m_faces) {
        P0 = m_vertices[face.v1];
        P1 = m_vertices[face.v2];
        P2 = m_vertices[face.v3];
        // NOTE: triangle intersection!!!!
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
