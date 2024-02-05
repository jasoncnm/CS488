// Termm--Fall 2020

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

typedef unsigned int u32;

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;

enum Axis {
    X, Y, Z
};

enum Modes {
    RView, TView, Perspective, RModel, TModel, SModel, Viewport, num_modes
};

// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
    VertexData();

    std::vector<glm::vec2> positions;
    std::vector<glm::vec3> colours;
    GLuint index;
    GLsizei numVertices;
};

struct LineIndex {
    u32 left;
    u32 right;
};

class A2 : public CS488Window {
public:
    A2();
    virtual ~A2();

protected:
    virtual void init() override;
    virtual void appLogic() override;
    virtual void guiLogic() override;
    virtual void draw() override;
    virtual void cleanup() override;

    virtual bool cursorEnterWindowEvent(int entered) override;
    virtual bool mouseMoveEvent(double xPos, double yPos) override;
    virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
    virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
    virtual bool windowResizeEvent(int width, int height) override;
    virtual bool keyInputEvent(int key, int action, int mods) override;

    void createShaderProgram();
    void enableVertexAttribIndices();
    void generateVertexBuffers();
    void mapVboDataToVertexAttributeLocation();
    void uploadVertexDataToVbos();

    void initLineData();
    void OrthDraw();
    void Reset();
    void clip(glm::vec4 left, glm::vec4 right);
    glm::vec2   WindowToViewPort(glm::vec2 pw);
    glm::mat4   viewMatrix();
    glm::mat4   Translation(float dx, float dy, float dz);
    glm::mat4   Scale (float sx, float sy, float sz);
    glm::mat4   RotationOnAxis(float theta, Axis axi);
    glm::mat3x4 OrthographicMode();
    
    void setLineColour(const glm::vec3 & colour);
    void drawLine (
            const glm::vec2 & v0,
            const glm::vec2 & v1
    );

    ShaderProgram m_shader;

    GLuint m_vao;            // Vertex Array Object
    GLuint m_vbo_positions;  // Vertex Buffer Object
    GLuint m_vbo_colours;    // Vertex Buffer Object

    VertexData m_vertexData;

    glm::mat4 model;
    glm::mat4 modelTransform;
    
    glm::mat4 view;
    glm::mat4 viewTransform;
    glm::vec4 lookFrom, lookAt, up;
    
    glm::vec3 m_currentLineColour;
    glm::vec4 cube[8];
    
    
    LineIndex cubeLines[12];
    bool mouseButtonActive = false;
    bool dX, dY, dZ;
    float mTranslateX, mTranslateY, mTranslateZ;
    float mRotateX, mRotateY, mRotateZ;
    float mScaleX = 250, mScaleY = 250, mScaleZ = 250;
    float deltaX;
    float xPrev;
    Modes mode = Modes::RModel;
    
};

