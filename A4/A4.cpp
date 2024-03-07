// Termm--Fall 2020

#include <glm/ext.hpp>

#include "A4.hpp"

using namespace glm;

static vec3 RayColour(Ray ray, vec2 uv, uint maxhit) {

    vec3 colour;


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

    float d = (float)h / (2 * tan(radians(fovy / 2)));
    mat4 T1 = translate(mat4(1.), vec3(-0.5f * (float)w, -0.5f * (float)h, d)); 
    mat4 S2 = scale(mat4(1.), vec3(-1, 1, 1));

    vec3 view_z = normalize(view - eye);
    vec3 view_x = normalize(cross(up, view_z));
    vec3 view_y = cross(view_z, view_x);

    mat4 R3 = mat4(vec4(view_x, 0), vec4(view_y, 0), vec4(view_z, 0), vec4(0,0,0,1));
    mat4 T4 = translate(mat4(1.), eye);
    vec3 colour(1,1,1);
    for (uint y = 0; y < h; ++y) {
        for (uint x = 0; x < w; ++x) {
            vec4 pw = T4 * R3 * S2 * T1 * vec4((float)x, h - 1 - (float)y, 0, 1);
            Ray ray;
            ray.origin = vec4(eye, 1);
            ray.direction = pw - ray.origin;
            image(x, y, 0) = pw.x < 0 ? 0 : pw.x / (0.5f * w);
            image(x, y, 1) = pw.y < 0 ? 0 : pw.y / (0.5f * h);
            image(x, y, 2) = 0;

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
