// Termm--Fall 2020

#define SSAA

#include <glm/ext.hpp>

#include "A4.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Mesh.hpp"
using namespace glm;

typedef unsigned int uint;

bool once = true;
const static float tol = 0.0001f;
#ifdef SSAA
const static float minhit = 0.0005;
#else
const static float minhit = 0.005f;
#endif
void printmat4(mat4 mat) {
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            float f = mat[col][row];
            if (f <= tol && f >= -tol) {
                f = 0;
            }
            std::cout << f << "\t";
        }
        std::cout << std::endl;
    }
}

static vec3 Reflect(vec3 d, vec3 n) {

    vec3 result = d - 2 * dot(d, n) * n;
#if 0
    float cos1 = dot(-d, n) / (length(-d) * length(n));
    float cos2 = dot(result, n) / (length(result) * length(n));

    std::cout << "cos1 " << cos1 << " cos2 " << cos2  << " cos1 angle " << degrees(acos(cos1)) << std::endl;
#endif    
    return result;
}

static bool Hit(SceneNode * root, Ray & ray, HitRecord & record, MatrixStack & stack) {
    bool hit = false;
    stack.push(root->trans);
    if (root->m_nodeType == NodeType::GeometryNode) {

        GeometryNode * gnode = static_cast<GeometryNode *>(root);
        vec3 e = vec3(stack.inv * vec4(ray.origin, 1));
        vec3 d = vec3(stack.inv * vec4(ray.direction, 0));
        float l = length(vec3(stack.inv[0][0], stack.inv[1][1], stack.inv[2][2]));
        //float epi = l * minhit;
        float epi = minhit;

        switch (gnode->m_primitive->type) {
            case PrimitiveType::NHSPHERE: {
                NonhierSphere * sphere = static_cast<NonhierSphere *>(gnode->m_primitive);
                hit = sphere->Hit(e, d, record, epi);
                break;
            }
            case PrimitiveType::NHBOX: {
                NonhierBox * box = static_cast<NonhierBox *>(gnode->m_primitive);
                hit = box->Hit(e, d, record, epi);                
                
                break;
            }
            case PrimitiveType::MESH: {
                Mesh * mesh = static_cast<Mesh *>(gnode->m_primitive);
                hit = mesh->Hit(e, d, record, epi);
                break;
            }
            case PrimitiveType::CUBE: {
                Cube * cube = static_cast<Cube *>(gnode->m_primitive);
                hit = cube->Hit(e, d, record, epi);
                break;
            }
            case PrimitiveType::SPHERE: {
                Sphere * sphere = static_cast<Sphere *>(gnode->m_primitive);
                hit = sphere->Hit(e, d, record, epi);
                break;
            }
                    
            default:
                break;
        } // switch
        if (hit) {
            PhongMaterial * mat = static_cast<PhongMaterial *>(gnode->m_material);
            record.kd = mat->getkd();
            record.ks = mat->getks();
            record.shininess = mat->getshininess();                
            record.hit_point = vec3(stack.M * vec4(record.hit_point, 1));
            record.normal = vec3(transpose(stack.inv) * vec4(record.normal, 0));
            record.normal = normalize(record.normal);
        }

    }
    
    for (SceneNode * node : root->children) {
        hit |= Hit(node, ray, record, stack);
    } // for
    
    stack.pop(root->invtrans);
#if 0
    if (once) {
        std::cout << "----------------------------------------" << std::endl;
        printmat4(stack.M * stack.inv);
        std::cout << "----------------------------------------" << std::endl;
    }
#endif 
    return hit;
}

static bool Hit(SceneNode * root, Ray & ray, MatrixStack & stack) {
    stack.push(root->trans);
    bool hit = false;
    if (root->m_nodeType == NodeType::GeometryNode) {

        GeometryNode * gnode = static_cast<GeometryNode *>(root);
        float l = length(vec3(stack.inv[0][0], stack.inv[1][1], stack.inv[2][2]));
        //float epi = l * minhit;    
        float epi = minhit;

        vec3 e = vec3(stack.inv * vec4(ray.origin, 1));
        vec3 d = vec3(stack.inv * vec4(ray.direction, 0));

        switch (gnode->m_primitive->type) {
            case PrimitiveType::NHSPHERE: {
                NonhierSphere * sphere = static_cast<NonhierSphere *>(gnode->m_primitive);
                if (sphere->Hit(e, d, epi)) {
                    hit = true;
                }
                break;
            }
            case PrimitiveType::NHBOX: {
                NonhierBox * box = static_cast<NonhierBox *>(gnode->m_primitive);
                if (box->Hit(e, d, epi)) {
                    hit = true;
                }                    
                break;
            }
            case PrimitiveType::MESH: {
                Mesh * mesh = static_cast<Mesh *>(gnode->m_primitive);
                if (mesh->Hit(e, d, epi)) {
                    hit = true;
                }
                break;
            }                
            case PrimitiveType::CUBE: {
                Cube * cube = static_cast<Cube *>(gnode->m_primitive);
                if (cube->Hit(e, d, epi)) {
                    hit = true;
                }
                break;
            }
            case PrimitiveType::SPHERE: {
                Sphere * sphere = static_cast<Sphere *>(gnode->m_primitive);
                if (sphere->Hit(e, d, epi)) {
                    hit = true;
                }
                break;
            }
        }
    }
   
    for (SceneNode * node : root->children) {
        hit |= Hit(node, ray, stack);
    }
    stack.pop(root->invtrans);
    return hit;
}


static vec3 DirectLight(SceneNode * root,
                        const vec3 & v,
                        HitRecord & record,
                        const Light * light,
                        MatrixStack & stack) {
    vec3 result(0);
    vec3 shadow_e = record.hit_point;
    vec3 shadow_d = normalize(light->position - record.hit_point);
    Ray shadow_ray = {record.hit_point, shadow_d};
    vec3 h = normalize(v + shadow_d);
    if (!Hit(root, shadow_ray, stack)) {
        double r = length(light->position - record.hit_point);
        double attenuation = 1.0 / ( light->falloff[0] + light->falloff[1] * r + light->falloff[2] * r * r );
        result = record.kd * max(0.0f, dot(record.normal, shadow_d)) * light->colour * attenuation
            + record.ks * pow(max(0.0f, dot(record.normal, h)), record.shininess) * light->colour * attenuation;
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
    MatrixStack stack = {mat4(1.), mat4(1.)};

    HitRecord record = { FLT_MAX, vec3(0), vec3(0), vec3(0), vec3(0), 0.0 };  
    if (Hit(root, ray, record, stack)) {
        // once = false;
        vec3 v = normalize(eye - record.hit_point);
        colour = record.kd * ambient;
        for (const Light * light : lights) {
            colour +=  0.8f * DirectLight(root, v, record, light, stack);            
        }
        
        if (maxhit < 5) {
            maxhit += 1;
            ray.origin = record.hit_point;
            ray.direction = normalize(Reflect(ray.direction, record.normal));
            colour += 0.5f * record.ks * RayColour(root, ray, maxhit, ambient, eye, lights);
        }
    } else {
        // NOTE: BG colour
        // once = false;
        float y = ray.direction.y;
        colour += (1.0 - y) * vec3(0.722, 0.306, 0.039) + y * vec3(0.0, 0.0, 0.0);
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

#ifdef SSAA
            float f = 1.0f/3.0f;
            vec2 offsets[9];
            offsets[0] = vec2( 0, 0);
            offsets[1] = vec2(-f, f);
            offsets[2] = vec2( 0, f);
            offsets[3] = vec2( f, f);
            offsets[4] = vec2(-f, 0);
            offsets[5] = vec2( f, 0);
            offsets[6] = vec2(-f,-f);
            offsets[7] = vec2( 0,-f);
            offsets[8] = vec2( f,-f);
            vec3 colour;
            for (int i = 0; i < 9; i++) {
                float _x = (float)x + offsets[i].x;
                float _y = (float)y + offsets[i].y;
                vec4 pw = T4 * R3 * S2 * T1 * vec4(_x, h - 1 - _y, 0, 1);            
                Ray ray;
                ray.origin = eye;
                ray.direction = normalize(vec3(pw) - ray.origin);

                if (x == 99 && y == 117) {
                    int a = 0;
                }
            
                colour += RayColour(root, ray, 0, ambient, eye, lights);
                
            }
            colour.x = colour.x / 9.0f;
            colour.y = colour.y / 9.0f;
            colour.z = colour.z / 9.0f;
            
#else
            vec4 pw = T4 * R3 * S2 * T1 * vec4((float)x, h - 1 - (float)y, 0, 1);            
            vec3 colour;
            Ray ray;
            ray.origin = eye;
            ray.direction = normalize(vec3(pw) - ray.origin);

            if (x == 99 && y == 117) {
                int a = 0;
            }
            
            colour = RayColour(root, ray, 0, ambient, eye, lights);
#endif

            colour.x = clamp(colour.x, 0.0f, 1.0f);
            colour.y = clamp(colour.y, 0.0f, 1.0f);
            colour.z = clamp(colour.z, 0.0f, 1.0f);
            if (colour.z > mx.z) mx = colour;
            
            image(x, y, 0) = colour.x;
            image(x, y, 1) = colour.y;
            image(x, y, 2) = colour.z;            

        }
        progress += step;
        std::cout << '\r'  << "progress: " << (int)progress << "%"  << std::flush;
    }
    std::cout << "\nDone!"  << std::endl;
    
//    std::cout << std::endl << "max " << mx.z << std::endl;
}
