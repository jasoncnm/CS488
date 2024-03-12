// Termm--Fall 2020

#include <glm/ext.hpp>

#include "A4.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Mesh.hpp"
using namespace glm;

typedef unsigned int uint;

static vec3 Reflect(vec3 d, vec3 n) {

    vec3 result = d - 2 * dot(d, n) * n;
#if 0
    float cos1 = dot(-d, n) / (length(-d) * length(n));
    float cos2 = dot(result, n) / (length(result) * length(n));

    std::cout << "cos1 " << cos1 << " cos2 " << cos2  << " cos1 angle " << degrees(acos(cos1)) << std::endl;
#endif    
    return result;
}

static bool Hit(SceneNode * root, Ray & ray, HitRecord & record) {
    bool hit = false;
    for (SceneNode * node : root->children) {

        if (node->m_nodeType == NodeType::GeometryNode) {
            GeometryNode * gnode = static_cast<GeometryNode *>(node);
            switch (gnode->m_primitive->type) {
                case PrimitiveType::NHSPHERE: {
                    NonhierSphere * sphere = static_cast<NonhierSphere *>(gnode->m_primitive);
                    vec3 e = ray.origin;
                    vec3 d = ray.direction;
                    if (sphere->Hit(e, d, record)) {
                        hit = true;
                        PhongMaterial * mat = static_cast<PhongMaterial *>(gnode->m_material);
                        record.kd = mat->getkd();
                        record.ks = mat->getks();
                        record.shininess = mat->getshininess();
                    }
                    break;
                }
                case PrimitiveType::NHBOX: {
                    NonhierBox * box = static_cast<NonhierBox *>(gnode->m_primitive);
                    vec3 e = ray.origin;
                    vec3 d = ray.direction;
                    if (box->Hit(e, d, record)) {
                        hit = true;
                        PhongMaterial * mat = static_cast<PhongMaterial *>(gnode->m_material);
                        record.kd = mat->getkd();
                        record.ks = mat->getks();
                        record.shininess = mat->getshininess();                
                    }

                    break;
                }
                case PrimitiveType::MESH: {
                    Mesh * mesh = static_cast<Mesh *>(gnode->m_primitive);
                    vec3 e = ray.origin;
                    vec3 d = ray.direction;
                    if (mesh->Hit(e, d, record)) {
                        hit = true;
                        PhongMaterial * mat = static_cast<PhongMaterial *>(gnode->m_material);
                        record.kd = mat->getkd();
                        record.ks = mat->getks();
                        record.shininess = mat->getshininess();                
                    }
                    break;
                }
                    
                default:
                    break;
            } // switch

        }
        
    } // for
    return hit;
}

static bool Hit(SceneNode * root, Ray & ray) {
    for (SceneNode * node : root->children) {
        if (node->m_nodeType == NodeType::GeometryNode) {
            GeometryNode * gnode = static_cast<GeometryNode *>(node);
            switch (gnode->m_primitive->type) {
                case PrimitiveType::NHSPHERE: {
                    NonhierSphere * sphere = static_cast<NonhierSphere *>(gnode->m_primitive);
                    vec3 e = ray.origin;
                    vec3 d = ray.direction;
                    if (sphere->Hit(e, d)) {
                        return true;
                    }
                    break;
                }
                case PrimitiveType::NHBOX: {
                    NonhierBox * box = static_cast<NonhierBox *>(gnode->m_primitive);
                    vec3 e = ray.origin;
                    vec3 d = ray.direction;
                    if (box->Hit(e, d)) {
                        return true;
                    }                    
                    break;
                }
                case PrimitiveType::MESH: {
                    Mesh * mesh = static_cast<Mesh *>(gnode->m_primitive);
                    vec3 e = ray.origin;
                    vec3 d = ray.direction;
                    if (mesh->Hit(e, d)) {
                        return true;
                    }
                    break;
                }   
            }
        }
    }
    return false;
}


static vec3 DirectLight(SceneNode * root,
                        const vec3 & v,
                        HitRecord & record,
                        const Light * light) {
    vec3 result(0);
    vec3 shadow_e = record.hit_point;
    vec3 shadow_d = normalize(light->position - record.hit_point);
    Ray shadow_ray = {record.hit_point, shadow_d};
    vec3 h = normalize(normalize(v) + normalize(shadow_d));
    if (!Hit(root, shadow_ray)) {
        // double r = length(light->position - pos);
        // double attenuation = 1.0 / ( light->falloff[0] + light->falloff[1] * r + light->falloff[2] * r * r );
#if 0
        result = record.kd * max(0.0f, dot(record.normal, shadow_d)) * light->colour * attenuation
            + record.ks * pow(max(0.0f, dot(record.normal, h)), record.shininess) * light->colour * attenuation;
#endif
        result = record.kd * max(0.0f, dot(record.normal, shadow_d)) * light->colour
            + record.ks * pow(max(0.0f, dot(record.normal, h)), record.shininess) * light->colour;
        
        // result += record.kd * light->colour;
    }
    return result;
}

static vec3 RayColour(
    SceneNode * root,
    Ray & ray,
    uint maxhit,
    const vec3 ambient,
    const vec3 eye,
    const std::list<Light *> & lights)
{
    vec3 colour;

    HitRecord record = { FLT_MAX, vec3(0), vec3(0), vec3(0), vec3(0), 0.0 };  
    
    if (Hit(root, ray, record)) {
        colour = record.kd * ambient;
        for (const Light * light : lights) {
            colour +=  0.8f * DirectLight(root, eye - record.hit_point, record, light);
            
        }

#if 1
        // TODO: DEBUG THIS !!!!
        if (maxhit < 8) {
            maxhit += 1;
            ray.origin = record.hit_point;
            ray.direction = normalize(Reflect(ray.direction, record.normal));
            colour += 0.5f * record.ks * RayColour(root, ray, maxhit, ambient, eye, lights);
        }
#endif        
    } else {
        // NOTE: BG colour
        float y = ray.direction.y;
        colour += (1.0 - y) * vec3(0.039, 0.106, 0.722) + y * vec3(0.0, 0.0, 0.0);
    }
    return colour;
}

void A4_Render(
    // What to render  
    SceneNode * root,

    // Image to write to, set to a given width and height  
    Image & image,

    // Viewing parameters  
    const glm::vec3 & eye,
    const glm::vec3 & view,
    const glm::vec3 & up,
    double fovy,

    // Lighting parameters  
    const glm::vec3 & ambient,
    const std::list<Light *> & lights
               ) {

    // Fill in raytracing code here...
    
    std::cout << "F20: Calling A4_Render(\n" <<
        "\t" << *root <<
        "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
        "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
        "\t" << "view: " << glm::to_string(view) << std::endl <<
        "\t" << "up:   " << glm::to_string(up) << std::endl <<
        "\t" << "fovy: " << fovy << std::endl <<
        "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
        "\t" << "lights{" << std::endl;

    for(const Light * light : lights) {
        std::cout << "\t\t" <<  *light << std::endl;
    }
    std::cout << "\t}" << std::endl;
    std:: cout <<")" << std::endl;

    size_t h = image.height();
    size_t w = image.width();

    float progress = 0;
    float step = 1.0f/(float)h * 100;
    
    float d = (float)h / (2 * tan(radians(fovy / 2)));
    mat4 T1 = translate(mat4(1.), vec3(-0.5f * (float)w, -0.5f * (float)h, d)); 
    mat4 S2 = scale(mat4(1.), vec3(-1, 1, 1));

    vec3 view_z = normalize(view - eye);
    vec3 view_x = normalize(cross(up, view_z));
    vec3 view_y = cross(view_z, view_x);

    mat4 R3 = mat4(vec4(view_x, 0), vec4(view_y, 0), vec4(view_z, 0), vec4(0,0,0,1));
    mat4 T4 = translate(mat4(1.), eye);
    vec3 mx;
    for (uint y = 0; y < h; ++y) {
        for (uint x = 0; x < w; ++x) {
            
            vec3 colour;
            vec4 pw = T4 * R3 * S2 * T1 * vec4((float)x, h - 1 - (float)y, 0, 1);
            Ray ray;
            ray.origin = eye;
            ray.direction = normalize(vec3(pw) - ray.origin);

            if (x == 99 && y == 117) {
                int a = 0;
            }
            
            colour = RayColour(root, ray, 0, ambient, eye, lights);

            colour.x = clamp(colour.x, 0.0f, 1.0f);
            colour.y = clamp(colour.y, 0.0f, 1.0f);
            colour.z = clamp(colour.z, 0.0f, 1.0f);
            if (colour.z > mx.z) mx = colour;
            
            image(x, y, 0) = colour.x;
            image(x, y, 1) = colour.y;
            image(x, y, 2) = colour.z;

            
            
#if 0
            // Red: 
            image(x, y, 0) = (double)0;
            // Green: 
            image(x, y, 1) = (double)1.0;
            // Blue: 
            image(x, y, 2) = (double)1.0;
#endif
        }
        progress += step;
        std::cout << '\r'  << "progress: " << (int)progress << "%"  << std::flush;
    }
    std::cout << std::endl << "max " << mx.z << std::endl;
}
