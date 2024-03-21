// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

struct WorkOrder {
    const SceneNode * root;
    Image & image;
    const glm::vec3 & eye;
    const glm::vec3 ambient;
    const std::list<Light *> & lights;
    const glm::mat4 & trans;
    const glm::vec2 * offsets;
    uint xmin, onepass_xmax, ymin, onepass_ymax;
    size_t w, h; 
};

struct WorkQueue {
    std::vector<WorkOrder> work_orders;
    uint work_order_count;

    volatile uint next_work_order_index;
    volatile uint tile_retired_count;
};


struct MatrixStack {
    glm::mat4 M;
    glm::mat4 inv;
    void push(const glm::mat4 & trans) {
        M = M * trans;
        inv = glm::inverse(M);
    }
    void pop(const glm::mat4 & invtrans) {
        M = M * invtrans;
        inv = glm::inverse(M);
    }
};


struct Ray {
    glm::vec3 origin, direction;
};

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
);
