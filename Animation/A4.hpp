// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"


struct MatrixStack {
    glm::mat4 M;
    glm::mat4 inv;
    void push(glm::mat4 & trans) {
        M = M * trans;
        inv = glm::inverse(M);
    }
    void pop(glm::mat4 & invtrans) {
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
