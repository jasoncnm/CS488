// Termm--Fall 2020

#include <glm/ext.hpp>

#include "A4.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"

using namespace glm;

static bool Hit(SceneNode * root, vec3 & kd, vec3 & ks,
                double & shininess, Ray & ray, vec3 & normal, vec3 & hit_point) {
    bool hit = false;
    float min_t = 0;
    for (SceneNode * node : root->children) {
        if (node->m_nodeType == NodeType::GeometryNode) {
            GeometryNode * gnode = static_cast<GeometryNode *>(node);
            switch (gnode->m_primitive->type) {
                case PrimitiveType::NHSPHERE: {
                    NonhierSphere * sphere = static_cast<NonhierSphere *>(gnode->m_primitive);
                    vec3 e = vec3(ray.origin);
                    vec3 d = vec3(ray.direction);
                    if (sphere->Hit(e, d, normal, hit_point, min_t)) {
                        hit = true;
                        PhongMaterial * mat = static_cast<PhongMaterial *>(gnode->m_material);
                        kd = mat->getkd();
                        ks = mat->getks();
                        shininess = mat->getshininess();
                    }
                    break;
                }
            }
        }
    }
    return hit;
}

static bool Hit(SceneNode * root, Ray & ray) {
    for (SceneNode * node : root->children) {
        if (node->m_nodeType == NodeType::GeometryNode) {
            GeometryNode * gnode = static_cast<GeometryNode *>(node);
            switch (gnode->m_primitive->type) {
                case PrimitiveType::NHSPHERE: {
                    NonhierSphere * sphere = static_cast<NonhierSphere *>(gnode->m_primitive);
                    vec3 e = vec3(ray.origin);
                    vec3 d = vec3(ray.direction);
                    if (sphere->Hit(e, d)) {
                        return true;
                    }
                    break;
                }
            }
        }
    }
    return false;
}

// TODO: DEBUG THIS !!!!
static vec3 DirectLight(SceneNode * root,
                        const vec3 & pos,
                        const vec3 & normal,
                        const vec3 & kd,
                        const vec3 & ks,
                        const vec3 & v,
                        double & shininess,
                        const std::list<Light *> & lights) {
    vec3 result(0);
    for (const Light * light : lights) {
        vec3 shadow_e = pos;
        vec3 shadow_d = normalize(light->position - pos);
        Ray shadow_ray = {vec4(pos, 1), vec4(shadow_d, 0)};
        vec3 h = normalize(-vec3(v) + shadow_d);
        if (!Hit(root, shadow_ray)) {
            result += kd * light->colour * max(0.0f, dot(normal, shadow_d)) +
                ks * light->colour  * pow(max(0.0f, dot(normal, h)), shininess);
        }
    }

    return result;
}


static void RayColour(
    SceneNode * root,
    Ray & ray,
    vec2 & uv,
    vec3 & colour,
    uint maxhit,
    const vec3 ambient,
    const std::list<Light *> & lights)
{
    vec3 kd, ks, normal, hit_point;
    double shininess;

    if (Hit(root, kd, ks, shininess, ray, normal, hit_point)) {
        colour = kd * ambient;
        colour += kd * DirectLight(root, hit_point, normal,
                                   kd, ks, vec3(ray.direction),
                                   shininess, lights);
        
    }
    
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

    float d = (float)h / (2 * tan(radians(fovy / 2)));
    mat4 T1 = translate(mat4(1.), vec3(-0.5f * (float)w, -0.5f * (float)h, d)); 
    mat4 S2 = scale(mat4(1.), vec3(-1, 1, 1));

    vec3 view_z = normalize(view - eye);
    vec3 view_x = normalize(cross(up, view_z));
    vec3 view_y = cross(view_z, view_x);

    mat4 R3 = mat4(vec4(view_x, 0), vec4(view_y, 0), vec4(view_z, 0), vec4(0,0,0,1));
    mat4 T4 = translate(mat4(1.), eye);

    for (uint y = 0; y < h; ++y) {
        for (uint x = 0; x < w; ++x) {
            vec3 colour(0.3,0.3,0.7);
            vec4 pw = T4 * R3 * S2 * T1 * vec4((float)x, h - 1 - (float)y, 0, 1);
            Ray ray;
            ray.origin = vec4(eye, 1);
            ray.direction = pw - ray.origin;
            vec2 uv;
            RayColour(root, ray, uv, colour, 10, ambient, lights);

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
    }

}
